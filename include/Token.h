#pragma once

# include "common.h"

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


// Token
typedef struct {
	ListItem	list;
	TokenType	type;
	char		*text;	// for T_WORD type
}	Token;

void		Token_init(Token *t);
void		Token_print(Token *t);
void		Token_free(Token *t);


extern const int OP_LEN[];
extern const char* TokenType_repr[];
extern const char* TokenType_str[];
