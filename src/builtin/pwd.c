# include "builtin.h"

int	ft_pwd(Executor *e, Command *cmd) {
	char	buf[PATH_MAX];

	(void)e;
	(void)cmd;
	if (! getcwd(buf, sizeof(buf)))
		return (error(1, "builtin: getcwd failed"), -1);
	printf("%s\n", buf);
	return (0);
}