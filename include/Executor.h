# pragma once

# include "common.h"
# include "Job.h"
# include <sys/types.h>

# define MAX_JOBS 10

typedef struct {
	Job			*jobs[MAX_JOBS + 1]; // jobs table
	Job			*current_job;
	bool		interactive;
	int			tty_fd;
	pid_t		shell_pgid;
} Executor;

void Executor_init(Executor *e);
void Executor_clear(Executor *e);
void Executor_print(Executor *e);

void Executor_exe(Executor *e, ListHead *pipeline, char *line);

/* Jobs */

Job *Executor_jobs_get(Executor *e, char *str_idx);

void Executor_get_last_stopped_job(Executor *e);
void Executor_get_job_by_pgid(Executor *e, pid_t pgid);



/* utils */

void give_terminal_to(pid_t pgid, int tty_fd);