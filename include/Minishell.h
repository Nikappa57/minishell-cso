#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h>
# include <stdbool.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <unistd.h>
# include <string.h>
# include <stdlib.h>
# include <assert.h>

# include "LinkedList.h"

/* ERROR msg */

# define EXIT_MSG "exit\n"
# define ERR_ARG_NOT_ALLOW "Arguments not allowed\n"
# define ERR_MAX_LINE_LENGHT "lexer error: max line length\n"
# define ERR_UNCLOSED_QUITES "lexer error: Unclosed quotes!"
# define ERR_UNCLOSED_SLASH "lexer error: Unclosed back slash!"
# define ERR_UNCLOSED_PIPE "Unclosed pipe!"

/* ERROR code */

# define EXIT_ERROR 1

/* others */

# define MAX_CMDS 64 // Prevent fork bombing
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
	T_NONE,			// useful for fun return in some cases and lexer enf of line
	
} TokenType;

extern const int OP_LEN[];
extern const char* TokenType_repr[];
extern const char* TokenType_str[];

// Token
typedef struct {
	ListItem	list;
	TokenType	type;
	char		*text;	// for T_WORD type
}	Token;

void		Token_init(Token *t);
void		Token_print(Token *t);
void		Token_free(Token *t);

// Lexer

void		Lexer_init(ListHead *l);
int			Lexer_line(ListHead *l, char *str);
void		Lexer_print(ListHead *l);
void		Lexer_clear(ListHead *l);

void		lexer_test();

// no expansion marker
# define NO_EXPAND_MARK '\x1D'

/* Parser */

// Command

typedef enum {
	R_IN,	// same number of TokenType
	R_OUT,
	R_APP,
	R_HD,
}	RedType;

extern const char* RedType_repr[];
extern const char* RedType_str[];

typedef struct {
	ListItem	list;
	RedType		type;
	char		*filename;
}	Redirection;


typedef struct {
	ListItem	list;
	char		**argv;	// argv[0] = cmd_name, argv[1:] = args, argv[argc] = None
	int			argc;
	// All redirection file will be created, only the last one used
	ListHead	redirections;
}	Command;

# define MAX_RED_PER_CMD 10

void		Command_init(Command *c);
void		Command_print(Command *c);
void		Command_free(Command *c);
void		Command_add_arg(Command *c, const char *s);
void		Command_add_redirection(Command *c, RedType type, const char *s);

// Parser

typedef struct {
	ListHead	cmd_list;
	Token		*current_token;
}	Parser;

void		Parser_init(Parser *p, ListHead *token_list);
int			Parser_pipeline(Parser *p);
void		Parser_clear(Parser *p);
void		Parser_print(Parser *p);

/*** utils ***/

TokenType	check_operator(char *s);
void		skip_ws(char **s);
bool		eol(char *s);
void		error(char *err, int code);
void		error_tok(char *err, char *token, int code);

RedType		token_to_red(TokenType t);

#endif