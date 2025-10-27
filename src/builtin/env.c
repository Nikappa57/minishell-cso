# include "builtin.h"
# include "env.h"

int	ft_env(Executor *e, Command *cmd) {
	(void)cmd;
	(void)e;
	g_exit_code = 0;
	env_print();
	return (0);
}
