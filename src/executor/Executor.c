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
		// give terminal to shell
		give_terminal_to(e->shell_pgid, e->tty_fd);
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
		ret = b_fn(e, cmd); // set g_exit_code
		if (ret != 0) fprintf(stderr, "buildin failed\n");
	}
	else
		g_exit_code = 0;

	if (DEBUG) printf("CMD code: %d\n", g_exit_code);

	// restore stdio
	restore_stdio();
	return ;
}

static void _Executor_child(Executor *e, Job *j, Command *cmd, int p_idx, int *pipes, int pipes_n) {
	int ret;

	// set signals
	set_child_signals();

	// set pgid
	// first: new group pgid = pid
	if (p_idx == 0) {
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
	if (pipes_n) {
		// in
		if (p_idx > 0) { // skip first for input
			assert(pipes[(p_idx - 1) * 2] > 2 && "Executor_exe | invalid pipe (in)");
			ret = dup2(pipes[(p_idx - 1) * 2], STDIN_FILENO);
			if (ret < 0) {
				error(1, "dup2(pipe in): %s", strerror(errno));
				_exit(g_exit_code);
			}
		}
		// out
		if (p_idx < j->pipeline->size - 1) { // skip last for output
			assert(pipes[p_idx * 2 + 1] > 2 && "Executor_exe | invalid pipe (out)");
			ret = dup2(pipes[p_idx * 2 + 1], STDOUT_FILENO);
			if (ret < 0) {
				error(1, "dup2(pipe out): %s", strerror(errno));
				_exit(g_exit_code);
			}
		}
		ret = close_pipes(pipes, pipes_n);
		if (ret == -1) {
			if (DEBUG) fprintf(stderr, "close pipe error");
			_exit(g_exit_code);
		}
	}

	(void)e;
	if (DEBUG) if (DEBUG) printf("*** CHILD ***\n");

	// redirections
	ret = redirections_apply(cmd);
	if (ret == -1) return ;
	if (cmd->argc == 0) {
		g_exit_code = 0;
		return ;
	}

	// builtin
	builtin_fn b_fn = find_builtin(cmd);
	if (b_fn) {
		ret = b_fn(e, cmd); // set g_exit_code
		if (ret != 0) fprintf(stderr, "buildin failed\n");
		return ;
	}

	execvp(cmd->argv[0], cmd->argv);
	if (errno == ENOENT)
		error(127, "Command not found: %s", cmd->argv[0]);
	else
		error(126, "Command error: %s: ", cmd->argv[0], strerror(errno));
}

static void	_Executor_wait_job(Executor *e, Job *j) {
	int		last_status	= 0;
	int		process_n	= j->process.size;
	pid_t	last_pid	= ((Process *) j->process.last)->pid;

	while (process_n > 0) {
		int status = 0;

		// wait for any process in the group
		pid_t w_pid = waitpid(-j->pgid, &status, WUNTRACED);
		if (w_pid < 0) {
			if (errno == EINTR) continue ;
			return ; // no process to wait
		}

		// CTR-Z : job stopped
		if (WIFSTOPPED(status)) {
			j->state = JOB_STOPPED; // update state
			fprintf(stderr, "\n[%d]  Stopped\n", (int)j->pgid);
			g_exit_code = (unsigned char)(128 + WSTOPSIG(status)); // 148 for SIGTSTP
			return ; // leave the job in the jobs table
		}

		// update Process status
		for (ListItem *it = j->process.first; it; it = it->next) {
			Process *p = (Process *) it;
			assert(p && "Job_wait | casting error");

			// find the process by id
			if (p->pid == w_pid) {
				// update status
				p->status = status;
				break ;
			}
		}

		// if process is finished, reduce process_n
		if (WIFEXITED(status) || WIFSIGNALED(status)) {
			// save last command status
			if (w_pid == last_pid) last_status = status;
			--process_n;
		}
	}

	assert(process_n == 0 && "_Executor_wait_job | process_n is not zero");

	// set job as done
	// process_n = 0 => job is done
	j->state = JOB_DONE;

	// update exit code
	// terminated normally
	if (WIFEXITED(last_status))
		g_exit_code = (unsigned char) WEXITSTATUS(last_status);
	// killed by signals
	else if (WIFSIGNALED(last_status)) {
		int sig = WTERMSIG(last_status);
		g_exit_code = (unsigned char)(128 + sig);
		if (sig == SIGINT) fprintf(stderr, "\n");
		if (sig == SIGQUIT) fprintf(stderr, "Quit (core dumped)\n");
	}
	else
		g_exit_code = 1;

	// remove job from jobs table and free obj
	List_detach(&e->jobs, &j->list);
	Job_clear(j);
	free(j);
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
		return (_Executor_parent(e, first, b_fn));

	// create pipes
	int pipes_n = (pipeline->size == 1) ? 0 : pipeline->size - 1;
	int pipes[2 * (pipes_n > 0 ? pipes_n : 1)];
	ret = create_pipes(pipes, pipes_n);
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
		Command *c = (Command *) it;
		assert(first && "Executor_exe | invalid cast");

		// ignore signals
		set_shell_signals_exe();

		pid_t pid = fork();
		if (pid == -1) { // error
			close_pipes(pipes, pipes_n);
			error(1, "fork error: %s", strerror(errno));
			j->state = JOB_DONE;
			return ;
		}
		// child
		else if (pid == 0) {
			_Executor_child(e, j, c, idx, pipes, pipes_n);
			Pipeline_clear(pipeline);
			Executor_clear(e);
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
	ret = close_pipes(pipes, pipes_n);
	if (ret == -1) {
		if (DEBUG) fprintf(stderr, "close pipe error");
		return ;
	}

	// give terminal to the new process
	if (e->interactive) give_terminal_to(j->pgid, e->tty_fd);

	// wait job
	_Executor_wait_job(e, j);

	// give terminal to shell
	if (e->interactive) give_terminal_to(e->shell_pgid, e->tty_fd);
}

void Executor_print(Executor *e) {
	printf("*** Executor ***\n");
	printf("interactive: %s\n", e->interactive ? "yes" : "no");
	printf("tty fd: %d | shell pgid: %d\n", e->tty_fd, e->shell_pgid);
	printf("JOBS table:\n");
	for (ListItem *it = e->jobs.first; it; it = it->next) {
		Job *j = (Job *) it;
		assert(j && "Executor_print | invalid cast");
		Job_print(j);
	}
}