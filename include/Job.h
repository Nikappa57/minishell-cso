# pragma once

# include "common.h"
# include "Process.h"
# include <sys/types.h>
# include <sys/wait.h>

typedef enum { JOB_RUNNING, JOB_STOPPED, JOB_DONE } JobState;

typedef struct Job {
	ListItem	list;
	JobState	state;
	ListHead	process; // Process list
	pid_t		pgid;
	bool		background;
} Job;

void	Job_init(Job *j, ListHead *pipeline);
void	Job_clear(Job *j);
void	Job_add_process(Job *j, pid_t pid);