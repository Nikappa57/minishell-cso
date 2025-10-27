# include "builtin.h"
# include "env.h"

int	ft_unset(Executor *e, Command *cmd) {
	(void)e;
	g_exit_code = 0;
	for (int i = 1; cmd->argv[i]; ++i)
		unsetenv(cmd->argv[i]);
	return (0);
}