# pragma once

# include "common.h"
# include "Command.h"
# include "Executor.h"
# include <linux/limits.h>

typedef int (*builtin_fn)(Executor *e, Command *cmd);

typedef struct {
	const char	*name;
	builtin_fn	fn;
} builtin;

builtin_fn	find_builtin(Command *cmd);

int			ft_cd(Executor *e, Command *c);
int			ft_export(Executor *e, Command *c);
int			ft_env(Executor *e, Command *c);
int			ft_unset(Executor *e, Command *c);
int			ft_exit(Executor *e, Command *c);
int			ft_echo(Executor *e, Command *c);
int			ft_pwd(Executor *e, Command *c);

/* utils */

bool	str_isdigit(const char *s);
