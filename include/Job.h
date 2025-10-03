# pragma once

# include "common.h"
# include "Command.h"
# include <sys/types.h>
# include <sys/wait.h>

# define MAX_JOBS 32

typedef enum { JOB_RUNNING, JOB_STOPPED, JOB_DONE } JobState;

extern const char* JobState_str[];

typedef struct Job {
	JobState	state;			// the job state
	pid_t		pgid;			// gpid of child process
	pid_t		last_pid;		// last child process pid
	int			alive_process;	// number of child process
	bool		background;		// true if job is running in background
	ListHead	*pipeline;		// list of commands
	int			idx;			// index of job in the job table
	char		*cmd_str;		// input line (from readline)
	uint64_t	stop_rank;		// rank for stopped process
	uint64_t	bg_rank;		// rank for background process
} Job;

void	Job_init(Job *j, ListHead *pipeline, int idx, char *cmd_str);
void	Job_clear(Job *j);
void	Job_print(Job *j);
void	Job_add_process(Job *j, pid_t pid);

/* Jobs table */

typedef struct JobsTable {
	Job			*table[MAX_JOBS + 1]; // jobs table
	Job			*current;
	uint64_t	max_rank;
} JobsTable;

void	JobTable_init(JobsTable *t);
Job		*JobsTable_add(JobsTable *t, ListHead *pipeline, char *line);
Job		*JobsTable_get(JobsTable *t, char *str_idx);
void	JobsTable_update_current(JobsTable *t);
void	JobsTable_remove(JobsTable *t, Job *j);
void	JobsTable_destroy(JobsTable *t);
