#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdbool.h>
# include <readline/readline.h>
# include <readline/history.h>

/* ERROR msg */

# define EXIT_MSG "exit\n"
# define ERR_ARG_NOT_ALLOW "Arguments not allowed\n"

/* ERROR code */

# define EXIT_SUCCESS 1
# define EXIT_ERROR 1

/* others */

# define MAX_CMDS 100

#endif