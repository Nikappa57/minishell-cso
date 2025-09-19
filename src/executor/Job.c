# include "Job.h"

void	Job_init(Job *j, ListHead *pipeline) {
	(void)pipeline;

	j->list.next = j->list.prev = 0;
	j->state = JOB_RUNNING;
	j->background = false;
	j->pgid = -1;

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
}

void	Job_add_process(Job *j, pid_t pid) {
	Process *p = (Process *) malloc(sizeof(Process));
	if (! p) handle_error("Job_add_process | malloc error");
	p->pid = pid;
	p->list.next = p->list.prev = 0;
	List_insert(&j->process, j->process.last, &p->list);
}

int	Job_wait(Job *j) {
	int		last_status	= 0;
	int		process_n	= j->process.size;
	pid_t	last_pid	= ((Process *) j->process.last)->pid;

	while (process_n > 0) {
		int status = 0;

		// wait for any process in the group
		pid_t w_pid = waitpid(-j->pgid, &status, WUNTRACED);
		if (w_pid < 0) {
			if (errno == EINTR) continue ;
			return (last_status); // no process to wait
		}

		// CTR-Z : job stopped
		if (WIFSTOPPED(status)) {
			j->state = JOB_STOPPED; // update state
			fprintf(stderr, "\n[%d]  Stopped\n", (int)j->pgid);
			g_exit_code = (unsigned char)(128 + WSTOPSIG(status)); // 148 for SIGTSTP

			return (last_status); // leave the job in the jobs table
		}

		// update Process status
		for (ListItem *it = j->process.first; it; it = it->next) {
			Process *p = (Process *) it;
			assert(p && "Job_wait | casting error");

			// find the process by id
			if (p->pid == w_pid) {
				// update status
				p->status = status;
				break ;
			}
		}

		// if process is finished, reduce process_n
		if (WIFEXITED(status) || WIFSIGNALED(status)) {
			// save last command status
			if (w_pid == last_pid) last_status = status;
			--process_n;
		}
	}
	// process_n = 0 => job is done
	j->state = JOB_DONE;
	return (last_status);
}

