# include "builtin.h"
# include "env.h"

int	ft_unset(Command *cmd)
{
	for (int i = 1; cmd->argv[i]; ++i)
		env_unset(cmd->argv[i]);
	return (0);
}