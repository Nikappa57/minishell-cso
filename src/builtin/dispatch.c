# include "builtin.h"

static const builtin BUILTINS[] = {
	{"cd",		ft_cd},
	{"env",		ft_env},
	{"export",	ft_export},
	{"unset",	ft_unset},
	{"exit",	ft_exit},
	{"echo",	ft_echo},
	{"pwd",		ft_pwd},
	{"jobs",	ft_jobs},
	{"fg",		ft_fg},
	{"bg",		ft_bg},
	{0, 0}
};

builtin_fn find_builtin(Command *c){
	if (!c || c->argc == 0) return (0);
	for (int i=0; BUILTINS[i].name; ++i)
		if (strcmp(BUILTINS[i].name, c->argv[0]) == 0)
			return BUILTINS[i].fn;
	return (0);
}