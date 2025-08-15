#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdbool.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <unistd.h>
# include <string.h>

# include "LinkedList.h"

/* ERROR msg */

# define EXIT_MSG "exit\n"
# define ERR_ARG_NOT_ALLOW "Arguments not allowed\n"
# define ERR_MAX_LINE_LENGHT "max line length\n"
# define ERR_UNCLOSED_QUITES "Unclosed quotes!"
# define ERR_UNCLUSED_PIPE "Unclosed pipe!"
# define ERR_UNCLUSED_SLASH "Unclosed back slash!"

/* ERROR code */

# define EXIT_SUCCESS 1
# define EXIT_ERROR 1

/* others */

# define MAX_CMDS 100
# define MAX_LINE_LEN 4096

/* global vars */
extern unsigned char	g_exit_code;
extern bool				g_alive;

/*** structures ***/

/* lexer */

// Token type
typedef enum {
	// operator
	T_PIPE,			// |
	
	T_RED_IN,		// <
	T_RED_OUT,		// >
	T_RED_OUT_APP,	// >>
	T_HEREDOC,		// <<
	
	T_WORD,			// cmd name, args ecc
	T_NONE,			// usefull for fun return in some cases
	
} TokenType;

const int OP_LEN[] = {1, 1, 1, 2, 2};
const char* TOKEN_NAME[] = {"T_PIPE", "T_RED_IN", "T_RED_OUT", "T_RED_OUT_APP",
							"T_HEREDOC", "T_WORD", "T_NONE"};

// Token
typedef struct {
	ListItem		list;
	TokenType		type;
	char			*text;	// for T_WORD type
} Token;

void		Token_init(Token *t);
void		Token_print(Token *t);

// Lexer

void		Lexer_init(ListHead *l);
int			Lexer_line(ListHead *l, char *str);
int			Lexer_print(ListHead *l);
void		Lexer_clear(ListHead *l);


/*** utils ***/

TokenType	check_operator(char *s);
void		skip_ws(char **s);
bool		eol(char *s);
void		error(char *err, int code);
void		check(bool cond, char *name);

// quote mask
# define M_SINGLE_Q 1
# define M_DOUBLE_Q 2


#endif