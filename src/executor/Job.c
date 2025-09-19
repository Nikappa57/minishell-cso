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
			Process *p_item = (Process *) aux;
			assert(p_item && "Job_clear | invalid token cast");
			aux = aux->next;
			free(p_item);
		}
		List_init(&j->process);
	}
}

void	Job_add_process(Job *j, pid_t pid) {
	Process *p = (Process *) malloc(sizeof(Process));
	if (! p) handle_error("Job_add_process | malloc error");
	p->pid = pid;
	p->list.next = p->list.prev = 0;
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
		
		// save cmd last status
		if (it == j->process.last) {
			if (WIFEXITED(status))
				g_exit_code = WEXITSTATUS(status);
		}
	}
}

