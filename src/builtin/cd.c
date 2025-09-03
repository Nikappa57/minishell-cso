# include "builtin.h"
# include "env.h"

static int	update_path() {
	char	buf[PATH_MAX];

	if (! getcwd(buf, sizeof(buf)))
		return (1);
	env_export("OLDPWD", getenv("PWD"));
	env_export("PWD", buf);
	return (0);
}

int	ft_cd(Command *cmd)
{
	char	*path;

	if (cmd->argc == 1) {
		path = getenv("HOME");
		if (! path)
			return (error(1, "cd: HOME not set"), 0);
	}
	else if (cmd->argc != 2)
		return (error(1, "cd: too many arguments"), 0);
	else if (! strcmp(cmd->argv[1], "-")) {
		path = getenv("OLDPWD");
		if (! path)
			return (error(1, "cd: OLDPWD not set", NULL), 0);
	} else
		path = cmd->argv[1];

	if (!*path)
		return (0);
	int ret = chdir(path);
	if (ret == -1)
		return (error(1, "cd: %s: %s", strerror(errno), path), 0);
	return (update_path());
}
