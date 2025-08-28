# pragma once

# include "common.h"

typedef struct {
	ListItem	list;
	pid_t		pid;
	int			status; // waitpid
} Process;
