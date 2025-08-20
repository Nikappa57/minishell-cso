#include "Minishell.h"

void Parser_init(Parser *p, ListHead *token_list) {
	assert(p && "Parser_init | parser is Null");
	List_init(p->cmd_list);

	assert(token_list->size && token_list->first && "Parser_init | empty token list");
	p->current_token = (Token *) token_list->first;
}

void Parser_clear(Parser *p) {
	assert(p && "Parser_clear | parser is Null");
	if (p->cmd_list->size && p->cmd_list->first) {
		ListItem* aux = p->cmd_list->first;
		while (aux) {
			Command* c_item = (Command*)(aux);
			assert(c_item && "Parser_clear | invalid token cast");
			aux = aux->next;
			Command_free(c_item);
			free(c_item);
		}
		List_init(p->cmd_list);
	}

	p->current_token = NULL;
}

// return -1 if error, 0 otherwise
int Parser_item(Parser *p, Command *c, bool required) {
	assert(p && "Parser_item | parser is Null");
	assert(p->current_token && "Parser_item | current token is Null");
	assert(c && "Parser_item | command is Null");

	// text
	if (p->current_token->type == T_WORD) {
		Command_add_arg(c, p->current_token->text);
		p->current_token = (Token *) p->current_token->list.next; // next
		return 0;
	}
	// any redirection
	if ((p->current_token->type == T_RED_IN)
		|| (p->current_token->type == T_RED_OUT)
		|| (p->current_token->type == T_RED_OUT_APP)
		|| (p->current_token->type == T_HEREDOC)) {
			RedType rd = (RedType) p->current_token->type; // different enum, but same index
			p->current_token = (Token *) p->current_token->list.next; // next
			if (p->current_token->type != T_WORD) { // must be T_WORD
				error("Syntax error: expected text after redirection", 2);
				return (-1);
			}
			Command_add_redirection(c, rd, p->current_token->text);
			return (0);
		}
	return (required ? -1 : 0); // error if the item is expected
}

int Parser_line(Parser *p) {
	assert(p->current_token && "Parser_line | current token is Null");
	return (0);
}
