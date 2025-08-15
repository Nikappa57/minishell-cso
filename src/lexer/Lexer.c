# include "Minishell.h"

void Lexer_init(ListHead *l) {
	List_init(l);
}

/*
* return 0: OK, -1: ERR -> skip other steps
*/
int Lexer_line(ListHead *l, char *str) {
	if (!str || !*str)
		return -1;
	if (strlen(str) > MAX_LINE_LEN)
		return (error(ERR_MAX_LINE_LENGHT, EXIT_ERROR), -1);

	while (42) {
		Token *token = (Token *) malloc(sizeof(Token));
		assert(token && "malloc token");
		Token_init(token);

		skip_ws(&str);
		if (eol(str)) return 0;	// end

		// operator
		TokenType tt = check_operator(str);
		if (tt != T_NONE) {  // operator found
			str += OP_LEN[tt];  // skip operator token
			token->type = tt;
			List_insert(l, l->last, (ListItem *)token);
			continue ;
		}

		// word
		char		buf[MAX_LINE_LEN] = {0};
		size_t		i = 0;

		token->type = T_WORD;
		while (21) {
			// check if word is finished
			if (eol(str)
				|| isspace(*str)
				|| check_operator(str) != T_NONE)
				break;

			// check for '\'
			if (*str == '\\') {
				if (eol(str + 1))
					return (error(ERR_UNCLUSED_SLASH, EXIT_ERROR), -1);
				buf[i++] = *(++str);
				continue;
			}

			// check for " or '
			if (*str == '"' || *str == '\'') {
				char c = *str;
				// skip to next " / '
				// don't copy " to buf
				if (c == '"')
					++str;
				int j = 0;
				while (str[j++] != c) {
					if (eol(str + j))
						return (error(ERR_UNCLOSED_QUITES, EXIT_ERROR), -1);
				}
				if (c == '"')
					j--;
				strncpy(buf + i, str, j);
				i += j;
				continue;
			}
			// normal char
			buf[i++] = *str++;
		}
		buf[i] = 0;
		printf("BUF: %s\n", buf);
		token->text = (char *) malloc(i + 1);
		assert(token->text && "malloc token test");
		strncpy(token->text, buf, i + 1);
		// add to list
		List_insert(l, l->last, (ListItem *)token);
	}
	return (0);
}

void Lexer_print(ListHead *l) {
	ListItem* aux = l->first;
	int i = 0;

	printf("--- Lexer print ---\n");
	while (aux) {
		printf("[%d] ", i++);
		Token* t_item = (Token*)(aux);
		assert(t_item && "Lexer_print|ERROR, invalid cast");
		Token_print(t_item);
		aux = aux->next;
		if (aux)
			printf(" | ");
	}
	printf("\n");

}

void Lexer_clear(ListHead *l) {
	if (!l->size || !l->first) {
		return;
	}

	ListItem* aux = l->first;
	while (aux) {
		Token* t_item = (Token*)(aux);
		assert(t_item && "FloatList_destroy|ERROR, invalid cast");
		aux = aux->next;
		free(t_item);
	}
	List_init(l);
}