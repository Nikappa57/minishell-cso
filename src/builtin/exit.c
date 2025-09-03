# include "builtin.h"

/*
*	print error
*	exit with exit code
*/
int	ft_exit(Command *cmd) {
	fprintf(stderr, "exit\n");

	if ((cmd->argc >= 2) && (! str_isdigit(cmd->argv[1])))
		error(2, "exit: %s, numeric argument required", cmd->argv[1]);
	else if (cmd->argc > 2)
		error(1, "exit too many arguments");
	else if (cmd->argc == 2)
		g_exit_code = atoi(cmd->argv[1]) % 256;

	// exit
	g_alive = false;
	return (0);
}