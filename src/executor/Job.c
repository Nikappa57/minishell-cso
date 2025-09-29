# include "Job.h"

void	Job_init(Job *j, ListHead *pipeline, int idx, char *cmd_str) {
	j->state = JOB_RUNNING;
	j->background = false;
	j->pgid = -1;
	j->pipeline = pipeline;
	j->idx = idx;
	j->cmd_str = strdup(cmd_str ? cmd_str : "");

	// process list init
	List_init(&j->process);
}

void	Job_clear(Job *j) {
	// process list clear
	if (j->process.size && j->process.first) {
		ListItem *aux = j->process.first;
		while (aux) {
			Process *p_item = (Process *) aux;
			assert(p_item && "Job_clear | invalid token cast");
			aux = aux->next;
			free(p_item);
		}
		List_init(&j->process);
	}
	if (j->cmd_str) {
		free(j->cmd_str);
		j->cmd_str = 0;
	}
}

// debug print
void	Job_print(Job *j) {
	printf("JOB [%d] state: %s, background: %s\n",
		j->pgid, JobState_str[j->state], j->background ? "yes" : "no");
	printf("Process: ");
	for (ListItem *it = j->process.first; it; it = it->next) {
		Process *p = (Process *) it;
		assert(p && "Job_print | invalid cast");
		printf("%d ", p->pid);
	}
	printf("\n");
}

void	Job_add_process(Job *j, pid_t pid) {
	Process *p = (Process *) malloc(sizeof(Process));
	if (! p) handle_error("Job_add_process | malloc error");
	p->pid = pid;
	p->list.next = p->list.prev = 0;
	List_insert(&j->process, j->process.last, &p->list);
}
