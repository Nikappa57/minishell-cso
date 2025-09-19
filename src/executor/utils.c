# include "Executor.h"

void give_terminal_to(pid_t pgid, int tty_fd) {
	if (tty_fd >= 0) tcsetpgrp(tty_fd, pgid);
}
