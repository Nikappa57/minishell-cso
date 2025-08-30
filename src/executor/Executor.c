# include "Executor.h"
# include "Command.h"
# include "builtin.h"
# include "redirections.h"

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

void Executor_exe(Executor *e, ListHead *pipeline) {
	assert(pipeline && pipeline->size > 0 && "Invalid pipeline");
	int ret;
	Command *first = (Command *) pipeline->first;
	assert(first && "Executor_exe | invalid cast");

	builtin_fn b_fn = find_builtin(first);
	
	// run in parent process if is a built in (or only redirections) and there is only one cmd
	if ((b_fn || first->argc == 0) && pipeline->size == 1)
		return _Executor_parent(e, first, b_fn);

	// create pipes
	int n_pipes = (pipeline->size == 1) ? 0 : pipeline->size - 1;
	int pipes[n_pipes][2];
	ret = create_pipes(pipes, n_pipes);
	if (ret == -1) {
		if (DEBUG) fprintf(stderr, "create pipe error");
	}

	// TODO: execute pipeline

	ret = close_pipes(pipes, n_pipes);
	if (ret == -1) {
		if (DEBUG) fprintf(stderr, "create pipe error");
	}

	return error(127, "Command not found");

	(void)e;
}
