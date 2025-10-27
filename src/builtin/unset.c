# include "builtin.h"
# include "env.h"

int	ft_unset(Executor *e, Command *cmd) {
	(void)e;
	for (int i = 1; cmd->argv[i]; ++i)
		unsetenv(cmd->argv[i]);
	return (0);
}