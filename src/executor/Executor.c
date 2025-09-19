# include "Executor.h"
# include "Command.h"
# include "builtin.h"
# include "redirections.h"
# include "Pipeline.h"
# include "signals.h"

void Executor_init(Executor *e) {
	List_init(&e->jobs);

	// man: test whether a file descriptor refers to a terminal
	e->interactive = isatty(STDIN_FILENO);
	e->tty_fd = STDIN_FILENO;
	if (e->interactive) {
		// set gpid = pid for the shell process
		e->shell_pgid = getpid();
		int ret = setpgid(0, e->shell_pgid);
		if (ret == -1) handle_error("Executor_init | setpgid error");
		// the shell get the terminal
		give_terminal_to(e->tty_fd, e->shell_pgid);
		// ignore signals in shell process
		set_shell_signals();
	}
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

	if (DEBUG) printf("*** PARENT ***\n");

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
		ret = b_fn(cmd); // set g_exit_code
		if (ret != 0) fprintf(stderr, "buildin failed\n");
	}
	else
		g_exit_code = 0;

	if (DEBUG) printf("CMD code: %d\n", g_exit_code);

	// restore stdio
	restore_stdio();
	return ;
}

static void _Executor_child(Executor *e, Command *cmd) {
	(void)e;
	if (DEBUG) if (DEBUG) printf("*** CHILD ***\n");

	// redirections
	int ret = redirections_apply(cmd);
	if (ret == -1) return ;
	if (cmd->argc == 0) {
		g_exit_code = 0;
		return ;
	}

	// builtin
	builtin_fn b_fn = find_builtin(cmd);
	if (b_fn) {
		ret = b_fn(cmd); // set g_exit_code
		if (ret != 0) fprintf(stderr, "buildin failed\n");
		return ;
	}

	execvp(cmd->argv[0], cmd->argv);
	if (errno == ENOENT)
		error(127, "Command not found: %s", cmd->argv[0]);
	else
		error(126, "Command error: %s: ", cmd->argv[0], strerror(errno));
}

void Executor_exe(Executor *e, ListHead *pipeline) {
	assert(pipeline && pipeline->size > 0 && "Invalid pipeline");

	// reset exit code
	g_exit_code = 0;

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
	if (ret == -1) {
		if (DEBUG) fprintf(stderr, "create pipe error");
		return ;
	}

	// create new job
	Job *j = (Job*) calloc(1, sizeof(Job));
	Job_init(j, pipeline);
	List_insert(&e->jobs, e->jobs.last, &j->list);

	int idx = 0;
	for (ListItem *it = pipeline->first; it; it = it->next, ++idx) {
		Command *c = (Command*)it;
		assert(first && "Executor_exe | invalid cast");

		pid_t pid = fork();
		if (pid == -1) { // error
			close_pipes(pipes, n_pipes);
			error(1, "fork error: %s", strerror(errno));
			j->state = JOB_DONE;
			return ;
		}
		// child
		else if (pid == 0) {
			// set signals
			set_child_signals();

			// set pgid
			// first: new group pgid = pid
			if (idx == 0) {
				ret = setpgid(0, 0);
				if (ret == -1) {
					error(1, "setpgid (first): %s", strerror(errno));
					_exit(g_exit_code);
				}
			}
			// others: join in the first process group
			else {
				assert(j->pgid != -1 && "Executor_exe | Job pgid is not set");
				ret = setpgid(0, j->pgid);
				if (ret == -1) {
					error(1, "setpgid: %s", strerror(errno));
					_exit(g_exit_code);
				}
			}

			// set pipe
			if (n_pipes) {
				// in
				if (idx > 0) { // skip first for input
					assert(pipes[(idx - 1) * 2] > 2 && "Executor_exe | invalid pipe (in)");
					ret = dup2(pipes[(idx - 1) * 2], STDIN_FILENO);
					if (ret < 0) {
						error(1, "dup2(pipe in): %s", strerror(errno));
						_exit(g_exit_code);
					}
				}
				// out
				if (idx < pipeline->size - 1) { // skip last for output
					assert(pipes[idx * 2 + 1] > 2 && "Executor_exe | invalid pipe (out)");
					ret = dup2(pipes[idx * 2 + 1], STDOUT_FILENO);
					if (ret < 0) {
						error(1, "dup2(pipe out): %s", strerror(errno));
						_exit(g_exit_code);
					}
				}
				ret = close_pipes(pipes, n_pipes);
				if (ret == -1) {
					if (DEBUG) fprintf(stderr, "close pipe error");
					_exit(g_exit_code);
				}
			}
			_Executor_child(e, c);
			_exit(g_exit_code);
		}
		// parent
		else {
			// if first: save the new pgid
			if (idx == 0) j->pgid = pid;

			// set pgid of child also in the parent process
			ret = setpgid(pid, j->pgid);
			if (ret == -1) error(1, "setpgid (parent): %s", strerror(errno));

			// create a new process
			Job_add_process(j, pid);
		}
	}

	// close pipe
	ret = close_pipes(pipes, n_pipes);
	if (ret == -1) {
		if (DEBUG) fprintf(stderr, "close pipe error");
		return ;
	}

	// give terminal to the new process
	if (e->interactive) give_terminal_to(j->pgid, e->tty_fd);

	// wait job
	int last_status = Job_wait(j);

	// give terminal to shell
	if (e->interactive) give_terminal_to(e->shell_pgid, e->tty_fd);

	// if the job is done, update exit code and remove job
	if (j->state == JOB_DONE) {
		if (WIFEXITED(last_status))
			g_exit_code = (unsigned char) WEXITSTATUS(last_status);
		else if (WIFSIGNALED(last_status))
			g_exit_code = (unsigned char) (128 + WTERMSIG(last_status));
		else
			g_exit_code = 1;
		// remove job
		List_detach(&e->jobs, &j->list);
		Job_clear(j);
		free(j);
	} else {
		// JOB_STOPPED: lascialo in tabella per fg/bg
		// (puoi marcarlo background=false per ora)
	}
}
