# include "builtin.h"
# include "env.h"

int	ft_export(Command *cmd)
{
	if (cmd->argc == 1)
		return (ft_env(cmd));
	
	for (int i = 1; cmd->argv[i]; i++) {
		if (cmd->argv[i][0] == '=') {
			error(1, "export: `%s': not a valid identifier\n", cmd->argv[i]);
			continue;
		}

		char *value;
		char *key_end = strchr(cmd->argv[i], '=');

		if (key_end) {
			*key_end = 0;
			value = key_end + 1;
		} else {
			value = 0;
		}
		env_export(cmd->argv[i], value);
	}
	return (0);
}