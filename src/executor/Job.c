# include "Job.h"

void	Job_init(Job *j, ListHead *pipeline, int idx, char *cmd_str) {
	j->state = JOB_RUNNING;
	j->background = false;
	j->pgid = -1;
	j->pipeline = pipeline;
	j->idx = idx;
	j->cmd_str = strdup(cmd_str ? cmd_str : "");
	j->last_pid = -1;
	j->alive_process = 0;
	j->stop_rank = 0;
	j->bg_rank = 0;
}

void	Job_clear(Job *j) {
	if (j->cmd_str) {
		free(j->cmd_str);
		j->cmd_str = 0;
	}
}

// debug print
void	Job_print(Job *j) {
	printf("JOB [%d] state: %s, background: %s\n",
		j->pgid, JobState_str[j->state], j->background ? "yes" : "no");
	printf("Alive process: %d", j->alive_process);
	printf("\n");
}

void	Job_add_process(Job *j, pid_t pid) {
	j->last_pid = pid;
	++(j->alive_process);
	// TODO: add to pid-job map
}
