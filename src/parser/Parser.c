#include "Minishell.h"

void Parser_init(Parser *p, ListHead *token_list) {
	assert(p && "Parser_init | parser is Null");
	List_init(&p->cmd_list);

	assert(token_list->size && token_list->first && "Parser_init | empty token list");
	p->current_token = (Token *) token_list->first;
}

void Parser_clear(Parser *p) {
	assert(p && "Parser_clear | parser is Null");
	// free commands
	if (p->cmd_list.size && p->cmd_list.first) {
		ListItem* aux = p->cmd_list.first;
		while (aux) {
			Command* c_item = (Command*)(aux);
			assert(c_item && "Parser_clear | invalid token cast");
			aux = aux->next;
			Command_free(c_item);
			free(c_item);
		}
		List_init(&p->cmd_list);
	}

	p->current_token = NULL;
}

void Parser_print(Parser *p) {
	assert(p && "Parser_print | parser is Null");
	printf("Command list:\n");
	if (p->cmd_list.size && p->cmd_list.first) {
		ListItem* aux = p->cmd_list.first;
		int i = 0;
		while (aux) {
			Command* c_item = (Command*)(aux);
			assert(c_item && "Parser_clear | invalid token cast");
			printf("[%d] ", i++);
			Command_print(c_item);
			aux = aux->next;
		}
	}
}

void Parser_error(Parser *p) {
	assert(p && "Parser_error | parser is Null");
	assert(p->current_token && "Parser_error | current token is Null");

	const char *token_str;
	Token *t = p->current_token;

	if (t->type != T_WORD)
		token_str = TokenType_str[t->type];
	else
		token_str = t->text;
	g_exit_code = 2;
	fprintf(stderr, "Syntax error: unexpected token '%s'\n", token_str);
}

/*
	1 -> ok, and the cmd is updated
	0 -> ok, no update
	-1 -> error
*/
int Parser_item(Parser *p, Command *c) {
	assert(p && "Parser_item | parser is Null");
	assert(p->current_token && "Parser_item | current token is Null");
	assert(c && "Parser_item | command is Null");

	// text
	if (p->current_token->type == T_WORD) {
		Command_add_arg(c, p->current_token->text);
		p->current_token = (Token *) p->current_token->list.next; // next
		return (1);
	}
	// any redirection
	if ((p->current_token->type == T_RED_IN)
		|| (p->current_token->type == T_RED_OUT)
		|| (p->current_token->type == T_RED_OUT_APP)
		|| (p->current_token->type == T_HEREDOC)) {
		RedType rd = token_to_red(p->current_token->type);
		p->current_token = (Token *) p->current_token->list.next; // next
		if (p->current_token->type != T_WORD) // must be T_WORD
			return (Parser_error(p), -1);
		Command_add_redirection(c, rd, p->current_token->text);
		p->current_token = (Token *) p->current_token->list.next; // next
		return (1);
	}
	return (0); // error if item is required
}

/*
	0 -> ok
	-1 -> error
*/
int Parser_cmd(Parser *p) {
	assert(p && "Parser_cmd | parser is Null");
	assert(p->current_token && "Parser_cmd | current token is Null");

	Command *c = (Command *) malloc(sizeof(Command));
	assert(c && "Parser_cmd | malloc error");
	Command_init(c);

	// if cmd is required, item is required
	int ret = Parser_item(p, c);
	if (ret == -1) return (free(c), -1); // -1 error
	if (ret == 0) return (Parser_error(p), free(c), -1); // item is required

	while (ret == 1) {
		ret = Parser_item(p, c);
		if (ret == -1) return (free(c), -1);
	}
	List_insert(&p->cmd_list, p->cmd_list.last, &c->list);
	return (1);
}

/*
	0 -> ok
	-1 -> error
*/
int Parser_pipeline(Parser *p) {
	assert(p && "Parser_pipeline | parser is Null");
	assert(p->current_token && "Parser_pipeline | current token is Null");

	int ret = Parser_cmd(p);
	if (ret == -1) return (-1);
	while (p->current_token->type != T_NONE) {
		if (p->current_token->type != T_PIPE)
			return (Parser_error(p), -1);
		p->current_token = (Token *) p->current_token->list.next; // next
		ret = Parser_cmd(p);
		if (ret == -1) return (-1);
		if (p->cmd_list.size > MAX_CMDS)
			return (error("Parser: maximum number of commands exceeded", 1), -1);
	}
	return (0);
}

