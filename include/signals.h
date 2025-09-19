# pragma once

# include "common.h"

# include <signal.h>
# include <termios.h>
# include <unistd.h>
# include <fcntl.h>


void	set_shell_signals();
void	set_child_signals();
