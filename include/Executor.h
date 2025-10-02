# pragma once

# include "common.h"
# include "Job.h"
# include <sys/types.h>

typedef struct {
	JobsTable	jobs;
	bool		interactive;
	int			tty_fd;
	pid_t		shell_pgid;
} Executor;

void Executor_init(Executor *e);
void Executor_clear(Executor *e);
void Executor_print(Executor *e);

void Executor_exe(Executor *e, ListHead *pipeline, char *line);
void Executor_wait_job(Executor *e, Job *j);

/* utils */

void give_terminal_to(pid_t pgid, int tty_fd);