# include "builtin.h"
# include <signal.h>

int ft_fg(Executor *e, Command *cmd) {
	// check if shell is interactive
	if (!e->interactive)
		return (error(1, "fg: no job control"), 0);

	// find job by arg (the default is current job)
	char *str_idx = (cmd->argc == 1) ? "+" : cmd->argv[1];
	Job *j = JobsTable_get(&e->jobs, str_idx);

	// job not found
	if (!j || j->state == JOB_DONE)
		return (error(1, "fg: %s: no such job",
			!strcmp(str_idx, "+") ? "current" : str_idx), 0);

	// print cmd
	fprintf(stderr, "%s\n", j->cmd_str);
	
	// give terminal to job
	give_terminal_to(j->pgid, e->tty_fd);

	// update infos
	j->background = false;

	// if job was stopped, send SIGCONT
	if (j->state == JOB_STOPPED) {
		// man: If pid is less than -1, then sig is sent to every
		// process in the process group whose ID is -pid.
		int ret = kill(-j->pgid, SIGCONT);
		if (ret == -1) {
			// give terminal to shell
			give_terminal_to(e->shell_pgid, e->tty_fd);
			error(1, "fg: SIGCONT: %s", strerror(errno));
			return (1);
		}
	}

	// wait for the job
	Executor_wait_job(e, j);

	// give terminal to shell
	give_terminal_to(e->shell_pgid, e->tty_fd);

	return (0);
}