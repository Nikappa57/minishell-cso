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
		skip_ws(&str);
		if (eol(str)) break;	// end

		Token *token = (Token *) malloc(sizeof(Token));
		assert(token && "Lexer_line | malloc token");
		Token_init(token);

		// operator
		TokenType tt = check_operator(str);
		if (tt != T_NONE) {  // operator found
			str += OP_LEN[tt];  // skip operator token
			token->type = tt;
			// insert using token's list member
			List_insert(l, l->last, &token->list);
			continue;
		}

		// word
		char		buf[MAX_LINE_LEN + 1] = {0};
		size_t		i = 0;

		token->type = T_WORD;
		while (21) {
			// check if word is finished
			if (eol(str)
				|| isspace((unsigned char)*str)
				|| check_operator(str) != T_NONE)
				break;

			// backslash
			else if (*str == '\\') {
				if (eol(++str)) // skip slash and check if is not closed
					return (error(ERR_UNCLOSED_SLASH, EXIT_ERROR), free(token), -1);
				buf[i++] = *(str++); // save next char
			}

			// quotes
			else if ((*str == '"') || (*str == '\'')) {
				char c = *str++; // save quote type and skip it
				// save mark for expander
				buf[i++] = c == '\'' ? MARK_SQ : MARK_DQ;
				// skip to next quote
				while (*str != c) {
					if (eol(str))
						return (error(ERR_UNCLOSED_QUITES, EXIT_ERROR), free(token), -1);
					buf[i++] = (*str == '$') ? MARK_KEY : *str;
					str++; // skip char
				}
				// save mark for expander
				buf[i++] = c == '\'' ? MARK_SQ : MARK_DQ;
				str++; // skip last quote
			}

			// normal char
			else {
				buf[i++] = (*str == '$') ? MARK_KEY : *str;
				str++; // skip char
			}
		}
		buf[i] = 0;
		// printf("BUF: %s\n", buf);
		assert(i <= MAX_LINE_LEN && "Lexer_line | i > MAX_LINE_LEN");
		token->text = strndup(buf, i);
		assert(token->text && "Lexer_line | malloc token test");
		// add to list
		List_insert(l, l->last, &token->list);
	}
	Token *token = (Token *) malloc(sizeof(Token));
	assert(token && "Lexer_line | malloc token");
	Token_init(token); // Token type = T_NONE
	// add to list T_NONE as end of line
	List_insert(l, l->last, &token->list);
	return (0);
}

void Lexer_print(ListHead *l) {
	ListItem* aux = l->first;
	int i = 0;

	printf("--- Lexer print ---\n");
	while (aux) {
		printf("[%d] ", i++);
		Token* t_item = (Token*)(aux);
		assert(t_item && "Lexer_print | invalid cast");
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
		assert(t_item && "Lexer_clear | invalid token cast");
		aux = aux->next;
		Token_free(t_item);    // free allocated text
		free(t_item);          // free token struct
	}
	List_init(l);
}