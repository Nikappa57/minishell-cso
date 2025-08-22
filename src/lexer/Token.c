# include "Minishell.h"

void Token_init(Token *t) {
	t->text = 0;
	t->type = T_NONE;
	t->list.next = t->list.prev = 0;
}

void Token_print(Token *t) {
	// Print token type and, for words, the text in quotes
	printf("%s", TokenType_repr[t->type]);
	if (t->type == T_WORD) {
		assert(t->text && "Token_print|Text is null");
		printf(" :\"");
		for (int i = 0; t->text[i]; i++) {
			if (t->text[i] == QUOTE_S_MARK)
				printf("<SQ>");
			if (t->text[i] == QUOTE_D_MARK)
				printf("<DQ>");
			else
				putchar(t->text[i]);
		}
		printf("\"");
	}
}

void Token_free(Token *t) {
	if (t->text)
		free(t->text);
}