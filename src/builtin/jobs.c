# include "builtin.h"

void job_print(Executor *e, Job *j) {
	if (!j) return ;
	printf("[%d]%c %s\t%s\n",
		j->idx + 1, 						// jobs idx
		j == e->current_job ? '+' : ' ',	// is current job
		JobState_str[j->state],				// job state
		j->cmd_str							// raw cmd input line
	);
}

int ft_jobs(Executor *e, Command *cmd) {
	
	// no args -> print all jobs
	if (cmd->argc == 1) {
		for (int i = 0; i < MAX_JOBS; i++) {
			if (e->jobs[i])
				job_print(e, e->jobs[i]);
		}
	}

	// else, find jobs by ids
	for (int i = 1; i < cmd->argc; i++) {
		Job *j = Executor_jobs_get(e, cmd->argv[i]);
		if (!j)
			error(1, "jobs: %s: no such job", cmd->argv[i]);
		else
			job_print(e, j);
	}

	return (0);
}