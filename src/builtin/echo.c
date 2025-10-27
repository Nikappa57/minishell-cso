# include "builtin.h"

// echo (-n) args
// if -n -> no newline
int	ft_echo(Executor *e, Command *cmd) {
	char	**argv;
	bool	add_newline = true;

	(void)e;
	if (cmd->argc == 1) {
		printf("\n");
		return (0);
	}
	argv = cmd->argv + 1;
	if (*argv && (strcmp(*argv, "-n") == 0)) {
		add_newline = 0;
		argv++;
	}
	while (*argv) {
		printf("%s", *argv++);
		if (*argv)
			printf(" ");
	}
	if (add_newline)
		printf("\n");
	return (0);
}
