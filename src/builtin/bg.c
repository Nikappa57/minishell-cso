# include "builtin.h"
# include "utils.h"
# include <signal.h>

int ft_bg(Executor *e, Command *cmd) {
	// check if shell is interactive
	if (!e->interactive)
		return (error(1, "bg: no job control"), 0);

	// find job by arg (the default is current job)
	char *str_idx = (cmd->argc == 1) ? "+" : cmd->argv[1];
	Job *j = JobsTable_get(&e->jobs, str_idx);

	// job not found
	if (!j || j->state == JOB_DONE) {
		return (error(1, "bg: %s: no such job",
			!strcmp(str_idx, "+") ? "current" : str_idx), 0);
	}
	// job is already running
	if (j->state == JOB_RUNNING) {
		if (j->background)
			error(0, "bg: job %s already in background",
				!strcmp(str_idx, "+") ? "current" : str_idx);
		else
			error(1, "bg: job %s is running in foreground",
				!strcmp(str_idx, "+") ? "current" : str_idx);
		return (0);
	}
	
	// job is stopped

	// send SIGCOUNT
	if (kill(-j->pgid, SIGCONT) < 0)
		return (error(1, "bg: SIGCONT: %s", strerror(errno)), 1);

	// update infos
	j->state = JOB_RUNNING;
	j->background = true;
	j->bg_rank = ++(e->jobs.max_rank);
	str_append(&j->cmd_str, " &");
	if (e->jobs.current == j)
		JobsTable_update_current(&e->jobs);

	// send an info msg
	fprintf(stderr, "[%d] %s\n", j->idx + 1, j->cmd_str);

	return (0);
}