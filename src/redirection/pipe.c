# include "redirections.h"

int close_pipes(int pipes[][2], int n_pipes) {
	int ret = 0;
	for (int i = 0; i < n_pipes; ++i) {
		for (int j = 0; j < 2; ++j) {
			if (pipes[i][j] != -1) {
				if (close(pipes[i][j]) == -1)
					ret = -1;
				pipes[i][j] = -1;
			}
		}
	}
	if (ret == -1) error(1, "close error : %s", strerror(errno));
	return (ret);
}

int create_pipes(int pipes[][2], int n_pipes) {
	for (int i = 0; i < n_pipes; ++i) {
		pipes[i][0] = -1;
		pipes[i][1] = -1;
	}
	for (int i = 0; i < n_pipes; ++i) {
		if (pipe(pipes[i]) == -1) {
			error(1, "pipe error : %s", strerror(errno));
			close_pipes(pipes, n_pipes);
			return (-1);
		}
	}
	return (0);
}

