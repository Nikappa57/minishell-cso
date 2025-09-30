# pragma once

# include "common.h"
# include "Command.h"
# include <sys/types.h>
# include <sys/wait.h>

typedef enum { JOB_RUNNING, JOB_STOPPED, JOB_DONE } JobState;

extern const char* JobState_str[];

typedef struct Job {
	JobState	state;			// the job state
	pid_t		pgid;			// gpid of child process
	pid_t		last_pid;		// last child process pid
	int			alive_process;		// number of child process
	bool		background;		// true if job is running in background
	ListHead	*pipeline;		// list of commands
	int			idx;			// index of job in the job table
	char		*cmd_str;		// input line (from readline)
} Job;

void	Job_init(Job *j, ListHead *pipeline, int idx, char *cmd_str);
void	Job_clear(Job *j);
void	Job_print(Job *j);
void	Job_add_process(Job *j, pid_t pid);