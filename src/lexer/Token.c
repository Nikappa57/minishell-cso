# include "Minishell.h"

void Token_init(Token *t) {
	t->text = 0;
	t->type = T_NONE;
	t->list.next = t->list.prev = 0;
}

void Token_print(Token *t) {
	printf("%s", TOKEN_NAME[t->type]);
	if (t->type == T_WORD) {
		assert(t->text && "Token_print|Text is null");
		printf(" (%s)", t->text);
	}
}

void Token_free(Token *t) {
	if (t->text)
		free(t->text);
}