# include "Job.h"

void	Job_init(Job *j, ListHead *pipeline) {
	j->list.next = j->list.prev = 0;
	j->state = JOB_RUNNING;
	// process
	List_init(&j->process);
	(void)pipeline;
}

void	Job_clear(Job *j) {
	// process list clear
	if (j->process.size && j->process.first) {
		ListItem *aux = j->process.first;
		while (aux) {
			Job *j_item = (Job *) aux;
			assert(j_item && "Lexer_clear | invalid token cast");
			aux = aux->next;
			Job_clear(j_item);
			free(j_item);
		}
		List_init(&j->process);
	}
}

void	Job_add_process(Job *j, pid_t pid) {
	Process *p = (Process *) malloc(sizeof(Process));
	if (! p) handle_error("Job_add_process | malloc error");
	p->pid = pid;
	List_insert(&j->process, j->process.last, &p->list);
}

void	Job_wait(Job *j) {
	int status;
	int ret;

	for (ListItem *it = j->process.first; it; it = it->next){
		Process *p = (Process*)it;
		assert(p && "Job_wait | invalid cast 2");

		ret = waitpid(p->pid, &status, 0);
		if (ret < 0) continue ;
	}

	// save last status
	if (WIFEXITED(status))
		g_exit_code = WEXITSTATUS(status);
}

