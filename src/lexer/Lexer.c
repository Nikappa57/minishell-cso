# include "Lexer.h"

void Lexer_init(ListHead *l) {
	List_init(l);
}

static int _Lexer_line_word(ListHead *l, char **str_p) {
	// word
	char		buf[MAX_LINE_LEN + 1] = {0};
	size_t		i = 0;

	// create token
	Token *token = (Token *) malloc(sizeof(Token));
	if (! token) handle_error("_Lexer_line_word | malloc token");
	Token_init(token, T_WORD);

	char *str = *str_p;
	while (21) {
		// check if word is finished
		if (eol(str)
			|| isspace((unsigned char)*str)
			|| check_operator(str) != T_NONE)
			break;

		// backslash
		else if (*str == '\\') {
			if (eol(++str)) // skip slash and check if is not closed
				return (error(1, "lexer error: Unclosed back slash!"), free(token), -1);
			buf[i++] = *(str++); // save next char
		}

		// quotes
		else if ((*str == '"') || (*str == '\'')) {
			char c = *str++; // save quote type and skip it
			// save mark for expander
			buf[i++] = (c == '\'') ? MARK_SQ : MARK_DQ;
			// skip to next quote
			while (*str != c) {
				if (eol(str))
					return (error(1, "lexer error: Unclosed quotes!"), free(token), -1);

				// single quotes
				if (c == '\'')
					buf[i++] = *str++;
				// duoble quotes
				else {
					// handle backslash in double quotes
					if (str[0] == '\\' && (str[1] == '"' || str[1] == '\\' || str[1] == '$')) {
						buf[i++] = str[1]; // copy
						str += 2; // skip to next char
					}
					else {
						buf[i++] = (*str == '$') ? MARK_KEY : *str;
						str++; // skip char
					}
				}
			}
			// save mark for expander
			buf[i++] = (c == '\'') ? MARK_SQ : MARK_DQ;
			str++; // skip last quote
		}

		// normal char
		else {
			buf[i++] = (*str == '$') ? MARK_KEY : *str;
			str++; // skip char
		}
	}
	buf[i] = 0;
	assert(i <= MAX_LINE_LEN && "_Lexer_line_word | i > MAX_LINE_LEN");
	assert(token->text == 0 && "_Lexer_line_word | token text is not Null");
	token->text = strndup(buf, i);
	if (! token->text) handle_error("_Lexer_line_word | malloc token test");
	// add to list
	List_insert(l, l->last, &token->list);

	*str_p = str;
	return (0);
}

/*
* return 0: OK, -1: ERR -> skip other steps
*/
int Lexer_line(ListHead *l, char *str) {
	if (!str || !*str)
		return (-1);
	if (strlen(str) > MAX_LINE_LEN)
		return (error(1, "lexer error: max line length\n"), -1);

	while (42) {
		skip_ws(&str);
		if (eol(str)) break;	// end

		// operator
		TokenType tt = check_operator(str);
		if (tt != T_NONE) {  // operator found
			str += OP_LEN[tt];  // skip operator token

			// create token
			Token *token = (Token *) malloc(sizeof(Token));
			if (! token) handle_error("Lexer_line | malloc token");
			Token_init(token, tt);

			// insert new token in token list
			List_insert(l, l->last, &token->list);
			continue;
		}

		// T_WORD
		int ret = _Lexer_line_word(l, &str);
		if (ret == -1) return (-1);
	}
	// add to list T_NONE as end of line
	Token *token = (Token *) malloc(sizeof(Token));
	if (! token) handle_error("Lexer_line | malloc token");
	Token_init(token, T_NONE); // Token type = T_NONE
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
		return ;
	}

	ListItem *aux = l->first;
	while (aux) {
		Token *t_item = (Token*)(aux);
		assert(t_item && "Lexer_clear | invalid token cast");
		aux = aux->next;
		Token_free(t_item);
		free(t_item);
	}
	List_init(l);
}