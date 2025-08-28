# pragma once

# include "common.h"
# include "Command.h"

typedef int (*builtin_fn)(Command *cmd);

typedef struct {
	const char	*name;
	builtin_fn	fn;
} builtin;

builtin_fn	find_builtin(Command *cmd);

int			ft_cd(Command *c);
int			ft_export(Command *c);
int			ft_unset(Command *c);
int			ft_exit(Command *c);
int			ft_echo(Command *c);
int			ft_pwd(Command *c);
