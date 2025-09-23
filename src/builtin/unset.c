# include "builtin.h"
# include "env.h"

int	ft_unset(Executor *e, Command *cmd) {
	(void)e;
	for (int i = 1; cmd->argv[i]; ++i)
		env_unset(cmd->argv[i]); // set exit code if key error
	return (0);
}