# include "builtin.h"

int	ft_pwd(Executor *e, Command *cmd) {
	char	buf[PATH_MAX];

	(void)e;
	(void)cmd;
	g_exit_code = 0;
	if (! getcwd(buf, sizeof(buf)))
		return (error(1, "builtin: getcwd failed"), -1);
	printf("%s\n", buf);
	return (0);
}