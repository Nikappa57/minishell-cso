# include "builtin.h"
# include "env.h"

int	ft_env(Command *cmd) {
	(void)cmd;
	env_print();
	return (0);
}
