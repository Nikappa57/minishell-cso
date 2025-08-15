# include "Minishell.h"

void Token_init(Token *t) {
	t->text = 0;
	t->type = T_NONE;
}

void Token_print(Token *t) {
	printf(TOKEN_NAME[t->type]);
	if (t->type == T_WORD)
		printf(" (%s)", t->text);
}
