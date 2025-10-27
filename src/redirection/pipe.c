# include "redirections.h"

int close_pipes(int pipes[], int n_pipes) {
	int ret = 0;
	for (int i = 0; i < n_pipes; ++i) {
		for (int j = 0; j < 2; ++j) {
			if (pipes[i * 2 + j] != -1) {
				if (close(pipes[i * 2 + j]) == -1)
					ret = -1;
				pipes[i * 2 + j] = -1;
			}
		}
	}
	if (ret == -1) error(1, "close error : %s", strerror(errno));
	return (ret);
}

int create_pipes(int pipes[], int n_pipes) {
	for (int i = 0; i < n_pipes; ++i) {
		pipes[i * 2] = -1;
		pipes[i * 2 + 1] = -1;
	}
	for (int i = 0; i < n_pipes; ++i) {
		int ret = pipe(pipes + i * 2);
		if (ret == -1) {
			error(1, "pipe error : %s", strerror(errno));
			close_pipes(pipes, n_pipes);
			return (-1);
		}
	}
	return (0);
}
