# pragma once

# include <linux/limits.h>
# include "common.h"

# define MAX_SHLVL 1000

void		env_init();
void		env_print();
int			env_export(const char *name, const char *value);
void		expander_pipeline(ListHead *pipeline);

