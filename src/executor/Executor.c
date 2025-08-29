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

void Executor_exe(Executor *e, ListHead *pipeline) {
	assert(pipeline && pipeline->size > 0 && "Invalid pipeline");

	int ret;
	Command *first = (Command *) pipeline->first;
	assert(first && "Executor_exe | invalid cast");

	builtin_fn b_fn = find_builtin(first);
	
	// run in parent process if is a built in and there is only one cmd
	if (b_fn && pipeline->size == 1) {

		// apply redirections
		ret = save_stdio();
		if (ret == -1) {
			g_exit_code = 1;
			if (DEBUG) fprintf(stderr, "save_stdio error\n");
			return ;
		}

		ret = redirections_apply(first);
		if (ret == -1) {
			restore_stdio();
			g_exit_code = 1;
			if (DEBUG) fprintf(stderr, "redirections_apply error\n");
			return ;
		}
		
		// run cmd
		ret = b_fn(first);
		g_exit_code = ret & 0xFF;

		if (DEBUG) printf("CMD code: %d\n", g_exit_code);

		// restore stdio
		restore_stdio();
		return ;
	}

	return error("Command not found", 127);

	(void)e;
}
