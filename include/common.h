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
# include <stdint.h>

# include "LinkedList.h"

// Debug

#ifndef DEBUG
# define DEBUG 0
#endif

/* ERROR code */

# define EXIT_ERROR 1

/* macro */

#define handle_error(msg) \
	do { perror(RED msg RESET); exit(EXIT_FAILURE); } while (0)


/* global vars */
extern unsigned char	g_exit_code;
extern bool				g_alive;

// env
extern char	**environ;

// quotes marker
# define MARK_SQ '\x1D' // ' -> MARK_SQ
# define MARK_DQ '\x1E' // " -> MARK_DQ
# define MARK_KEY '\x1F' // $ -> MARK_KEY

// color
# define RESET "\033[0m"
# define GREEN "\033[1;32m"
# define RED "\033[1;31m"
# define BLUE "\033[1;34m"
# define CYAN "\033[1;36m"

// utils

void error(int code, const char *str, ...);
