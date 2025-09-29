# pragma once

# include "common.h"

void		env_init();
void		env_print();
int			env_export(const char *name, const char *value);
int			env_unset(const char *name);
void		expander_pipeline(ListHead *pipeline);

