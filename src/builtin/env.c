# include "builtin.h"
# include "env.h"

int	ft_env(Executor *e, Command *cmd) {
	(void)cmd;
	(void)e;
	env_print();
	return (0);
}
