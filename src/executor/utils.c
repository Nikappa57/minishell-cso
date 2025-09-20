# include "Executor.h"

void give_terminal_to(pid_t pgid, int tty_fd) {
	if (tty_fd >= 0) {
		int ret = tcsetpgrp(tty_fd, pgid);
		if (ret == -1) error(1, "give_terminal_to(%d, %d): %s", pgid, tty_fd, strerror(errno));
	}
}
