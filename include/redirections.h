# pragma once

# include "common.h"
# include "Command.h"
# include <fcntl.h>
# include <errno.h>

void	heredoc_pipeline(ListHead *pipeline);
int		redirections_apply(Command *c);
int		save_stdio();
int		restore_stdio();