# include "redirections.h"

static int save_in = -1;
static int save_out = -1;

int save_stdio() {
	save_in  = dup(STDIN_FILENO);
	save_out = dup(STDOUT_FILENO);
	if (save_in < 0 || save_out < 0) {
		restore_stdio();
		perror("dup error");
		return (-1);
	}
	return (0);
}

int restore_stdio() {
	int ret;

	if (save_in  >= 0) {
		ret = dup2(save_in,  STDIN_FILENO);
		if (ret < 0) return (perror("restore_stdio | dup2 error (in)"), -1);
		ret = close(save_in);
		if (ret < 0) return (perror("restore_stdio | close error (in)"), -1);
	}

	if (save_out >= 0) {
		ret = dup2(save_out,  STDOUT_FILENO);
		if (ret < 0) return (perror("restore_stdio | dup2 error (out)"), 1);
		ret = close(save_out);
		if (ret < 0) return (perror("restore_stdio | close error (out)"), -1);
	}
	save_in = save_out = -1;
	return (0);
}