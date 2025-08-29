# pragma once

# include "common.h"
# include "Job.h"
# include <sys/types.h>

typedef struct {
	ListHead	jobs;
	int			fdin;
	int			fdout;
} Executor;

void Executor_init(Executor *e);
void Executor_clear(Executor *e);

void Executor_exe(Executor *e, ListHead *pipeline);