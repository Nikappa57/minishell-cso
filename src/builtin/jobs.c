# include "builtin.h"

static void job_print(Executor *e, Job *j) {
	if (!j) return ;
	printf("[%d]%c %s\t%s\n",
		j->idx + 1, 						// jobs idx
		j == e->jobs.current ? '+' : ' ',	// is current job
		JobState_str[j->state],				// job state
		j->cmd_str							// raw cmd input line
	);
}

int ft_jobs(Executor *e, Command *cmd) {
	g_exit_code = 0;
	
	// no args -> print all jobs
	if (cmd->argc == 1) {
		for (int i = 0; i < MAX_JOBS; i++) {
			Job *j = e->jobs.table[i];
			if (j) {
				job_print(e, j);
				if (j->state == JOB_DONE)
					JobsTable_remove(&e->jobs, j);
			}
		}
	}

	// else, find jobs by ids
	for (int i = 1; i < cmd->argc; i++) {
		Job *j = JobsTable_get(&e->jobs, cmd->argv[i]);
		if (!j)
			error(1, "jobs: %s: no such job", cmd->argv[i]);
		else
			job_print(e, j);
	}

	return (0);
}