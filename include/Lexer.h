#pragma once

# include "common.h"
# include "Token.h"

# define MAX_LINE_LEN 4096

// Lexer

void		Lexer_init(ListHead *l);
int			Lexer_line(ListHead *l, char *str);
void		Lexer_print(ListHead *l);
void		Lexer_clear(ListHead *l);

void		lexer_test();

// utils

TokenType	check_operator(char *s);
void		skip_ws(char **s);
bool		eol(char *s);