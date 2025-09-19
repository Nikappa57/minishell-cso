# include "builtin.h"
# include "env.h"

int	ft_unset(Command *cmd) {
	for (int i = 1; cmd->argv[i]; ++i)
		env_unset(cmd->argv[i]); // set exit code if key error
	return (0);
}