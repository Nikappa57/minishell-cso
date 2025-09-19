#pragma once

# include <stdio.h>
# include <stdbool.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <unistd.h>
# include <string.h>
# include <stdlib.h>
# include <assert.h>
# include <errno.h>

# include "LinkedList.h"

// Debug

#ifndef DEBUG
# define DEBUG 0
#endif

/* ERROR code */

# define EXIT_ERROR 1

/* macro */

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)


/* global vars */
extern unsigned char	g_exit_code;
extern bool				g_alive;

// env
extern char	**environ;

// quotes marker
# define MARK_SQ '\x1D' // ' -> MARK_SQ
# define MARK_DQ '\x1E' // " -> MARK_DQ
# define MARK_KEY '\x1F' // $ -> MARK_KEY

// utils

void error(int code, const char *str, ...);
