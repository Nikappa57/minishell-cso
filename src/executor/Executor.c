# include "Executor.h"
# include "Command.h"
# include "builtin.h"
# include "redirections.h"
# include "Pipeline.h"

void Executor_init(Executor *e) {
	List_init(&e->jobs);
}

void Executor_clear(Executor *e) {
	// jobs list clear
	if (e->jobs.size && e->jobs.first) {
		ListItem *aux = e->jobs.first;
		while (aux) {
			Job *j_item = (Job *) aux;
			assert(j_item && "Lexer_clear | invalid token cast");
			aux = aux->next;
			Job_clear(j_item);
			free(j_item);
		}
		List_init(&e->jobs);
	}
}

static void _Executor_parent(Executor *e, Command *cmd, builtin_fn b_fn) {

	(void)e;

	// redirections

	int ret = save_stdio();
	if (ret == -1) {
		if (DEBUG) fprintf(stderr, "save_stdio error\n");
		return ;
	}

	ret = redirections_apply(cmd);
	if (ret == -1) {
		restore_stdio();
		if (DEBUG) fprintf(stderr, "redirections_apply error\n");
		return ;
	}
	
	// run cmd
	if (b_fn) {
		ret = b_fn(cmd);
		g_exit_code = ret & 0xFF;
	}
	

	if (DEBUG) printf("CMD code: %d\n", g_exit_code);

	// restore stdio
	restore_stdio();
	return ;
}

static int _Executor_child(Executor *e, Command *cmd) {
	(void)e;
	(void)cmd;
	if (DEBUG) printf("CHILD");

	// redirections
	int ret = redirections_apply(cmd);
	if (ret == -1) goto C_RET;
	if (cmd->argc == 0) goto C_RET;

	// builtin
	builtin_fn b_fn = find_builtin(cmd);
	if (b_fn) {
		ret = b_fn(cmd);
		g_exit_code = ret & 0xFF;
		goto C_RET;
	}

	execvp(cmd->argv[0], cmd->argv);
	if (errno == ENOENT)
		error(127, "Command not found: %s", cmd->argv[0]);
	else
		error(126, "Command error: %s: ", cmd->argv[0], strerror(errno));

C_RET:
	Command_free(cmd);
	free(cmd);
	Executor_clear(e);
	return g_exit_code;
}

void Executor_exe(Executor *e, ListHead *pipeline) {
	assert(pipeline && pipeline->size > 0 && "Invalid pipeline");
	int ret;
	Command *first = (Command *) pipeline->first;
	assert(first && "Executor_exe | invalid cast");

	builtin_fn b_fn = find_builtin(first);
	
	// run in parent process if it's a builtin (also only redirections) and there is only one cmd
	if ((b_fn || first->argc == 0) && pipeline->size == 1)
		return _Executor_parent(e, first, b_fn);

	// create pipes
	int n_pipes = (pipeline->size == 1) ? 0 : pipeline->size - 1;
	int pipes[2 * (n_pipes > 0 ? n_pipes : 1)];
	ret = create_pipes(pipes, n_pipes);
	printf("N_PIPES: %d\n", n_pipes);
	if (ret == -1) {
		if (DEBUG) fprintf(stderr, "create pipe error");
		return ;
	}

	// create new job
	Job j;
	Job_init(&j, pipeline);
	// List_insert(&e->jobs, e->jobs.last, &j.list);

	int idx = 0;
	for (ListItem *it = pipeline->first; it; it = it->next, ++idx) {
		printf("IDX: %d\n", idx);
		Command *c = (Command*)it;
		assert(first && "Executor_exe | invalid cast");

		pid_t pid = fork();
		if (pid == -1) { // error
			Job_clear(&j);
			close_pipes(pipes, n_pipes);
			error(1, "fork error: %s", strerror(errno));
			return ;
		}
		else if (pid == 0) { // child
			// set pipe
			if (n_pipes) {
				// in
				if (idx > 0) { // skip first for input
					assert(pipes[(idx - 1) * 2] > 2 && "Executor_exe | invalid pipe (in)");
					ret = dup2(pipes[(idx - 1) * 2], STDIN_FILENO);
					if (ret < 0) {
						perror("dup2(pipe in)");
						_exit(g_exit_code);
					}
				}
				// out
				if (idx < pipeline->size - 1) { // skip last for output
					assert(pipes[idx * 2 + 1] > 2 && "Executor_exe | invalid pipe (out)");
					ret = dup2(pipes[idx * 2 + 1], STDOUT_FILENO);
					if (ret < 0) {
						perror("dup2(pipe out)");
						_exit(g_exit_code);
					}
				}
				ret = close_pipes(pipes, n_pipes);
				if (ret == -1) {
					if (DEBUG) fprintf(stderr, "close pipe error");
					_exit(g_exit_code);
				}
			}
			_exit(_Executor_child(e, c));
		}
		else { // parent
			// create new process
			Job_add_process(&j, pid);
		}
	}
	// close pipe
	ret = close_pipes(pipes, n_pipes);
	if (ret == -1) {
		if (DEBUG) fprintf(stderr, "close pipe error");
		return ;
	}
	// wait job
	// List_detach(e->)
	Job_wait(&j); // set g_exit_code
	Job_clear(&j);


	// TODO: execute pipeline

	ret = close_pipes(pipes, n_pipes);
	if (ret == -1) {
		if (DEBUG) fprintf(stderr, "create pipe error");
		return ;
	}

	return ;

	(void)e;
}
