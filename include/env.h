# pragma once

# include "common.h"

void		env_init();
void		env_print();
int			env_export(const char *name, const char *value);
int			env_unset(const char *name);
void		expander_pipeline(ListHead *pipeline);

// utils

void		str_append(char **s1, const char* s2);
void		char_append(char **s1, char c);
