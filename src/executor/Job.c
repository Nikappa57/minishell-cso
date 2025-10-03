# include "Job.h"
# include "utils.h"

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
}

/* Jobs table */

void JobTable_init(JobsTable *t) {
	t->current = 0;
	t->max_rank = 0;
	for (int i = 0; i < MAX_JOBS + 1; i++)
		t->table[i] = 0;
}

void JobsTable_update_current(JobsTable *t) {
	Job			*best		= 0;
	uint64_t	best_rank	= 0;

	// find most recent stopped job
	for (int i = 0; i < MAX_JOBS; ++i) {
		Job *j = t->table[i];
		if (j && j->state == JOB_STOPPED && j->stop_rank >= best_rank) {
			best_rank = j->stop_rank;
			best = j;
		}
	}
	if (best) {
		t->current = best;
		return ;
	}

	// else find oldest running job (min bg rank)
	best_rank = t->max_rank;
	for (int i = 0; i < MAX_JOBS; ++i) {
		Job *j = t->table[i];
		if (j && j->state == JOB_RUNNING && j->background && j->bg_rank <= best_rank) {
			best_rank = j->bg_rank;
			best = j;
		}
	}
	t->current = best;
}

Job		*JobsTable_add(JobsTable *t, ListHead *pipeline, char *line) {
	// find first free idx
	int j_idx = -1;
	for (int i = 0; i < MAX_JOBS; i++) {
		if (t->table[i] == 0) {
			j_idx = i;
			break;
		}
	}
	if (j_idx == -1) {
		error(1, "jobs: table full");
		return (0);
	}

	Job *j = t->table[j_idx] = (Job *) calloc(1, sizeof(Job));
	if (!j) handle_error("_Executor_job_add | malloc error");

	Job_init(j, pipeline, j_idx, line);
	return (j);
}

Job *JobsTable_get(JobsTable *t, char *str_idx) {
	Job *j = 0;

	// current job
	if (! strcmp(str_idx, "+")) {
		j = t->current;
	}
	// get by id
	else if (str_isdigit(str_idx)) {
		int idx = atoi(str_idx) - 1;
		if (idx < MAX_JOBS && idx >= 0)
			j = t->table[idx];
	}

	return (j);
}

// remove job from jobs table, free obj and adjust current
void JobsTable_remove(JobsTable *t, Job *j) {
	if (! j) return ;
	// remove job from the jobs table
	t->table[j->idx] = 0;
	// check if it's the current job
	if (t->current == j)
		JobsTable_update_current(t);
	Job_clear(j);
	free(j);
}

void	JobsTable_destroy(JobsTable *t) {
	for (int i = 0; i < MAX_JOBS + 1; i++) {
		if (t->table[i]) JobsTable_remove(t, t->table[i]);
	}
	t->current = 0;
	t->max_rank = 0;
}