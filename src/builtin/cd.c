# include "builtin.h"
# include "env.h"

int	ft_cd(Command *cmd)
{
	char	*path;

	// cd
	if (cmd->argc == 1) {
		path = getenv("HOME");
		if (! path)
			return (error(1, "cd: HOME not set"), 0);
	}
	// cd arg1 arg2 ...
	else if (cmd->argc != 2)
		return (error(1, "cd: too many arguments"), 0);
	// cd -
	else if (! strcmp(cmd->argv[1], "-")) {
		path = getenv("OLDPWD");
		if (! path)
			return (error(1, "cd: OLDPWD not set", NULL), 0);
	}
	// cd <path>
	else
		path = cmd->argv[1];

	// change process directory
	int ret = chdir(path);
	if (ret == -1)
		return (error(1, "cd: %s: %s", strerror(errno), path), 0);

	// update PWD and OLDPWD
	char	buf[PATH_MAX];
	// get new pwd
	if (! getcwd(buf, sizeof(buf)))
		return (1);
	env_export("OLDPWD", getenv("PWD"));
	env_export("PWD", buf);
	return (0);
}
