# pragma once

# include "common.h"
# include "Token.h"
# include "Command.h"

# define MAX_RED_PER_CMD 10
# define MAX_CMDS 64 // Prevent fork bombing

typedef struct {
	ListHead	pipeline;
	Token		*current_token;
}	Parser;

void		Parser_init(Parser *p);
int			Parser_pipeline(Parser *p, ListHead *token_list);
void		Parser_clear(Parser *p);
void		Parser_print(Parser *p);

// utils

RedType		token_to_red(TokenType t);