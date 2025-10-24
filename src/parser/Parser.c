# include "Parser.h"
# include "Pipeline.h"

void Parser_init(Parser *p) {
	assert(p && "Parser_init | parser is Null");
	List_init(&p->pipeline);
	p->current_token = 0;
}

void Parser_clear(Parser *p) {
	assert(p && "Parser_clear | parser is Null");
	Pipeline_clear(&p->pipeline);
	p->current_token = 0;
}

void Parser_print(Parser *p) {
	assert(p && "Parser_print | parser is Null");

	printf("Current token: ");
	if (p->current_token) Token_print(p->current_token);
	printf("\n");
	Pipeline_print(&p->pipeline);
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
	error(2, "Syntax error: unexpected token '%s'\n", token_str);
}

/*
	1 -> ok, and an item is found
	0 -> ok, but no item is found
	-1 -> error

	item := T_WORD
		| T_RED_OUT		T_WORD
		| T_RED_OUT_APP	T_WORD
		| T_RED_IN		T_WORD
		| T_HEREDOC		T_WORD;
*/
int Parser_item(Parser *p, Command *c) {
	assert(p && "Parser_item | parser is Null");
	assert(p->current_token && "Parser_item | current token is Null");
	assert(c && "Parser_item | command is Null");

	// T_WORD
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
		// save redirection type
		RedType rd = token_to_red(p->current_token->type);
		p->current_token = (Token *) p->current_token->list.next; // next
		// must be T_WORD
		if (p->current_token->type != T_WORD)
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

	cmd := item {item};
*/
int Parser_cmd(Parser *p) {
	assert(p && "Parser_cmd | parser is Null");
	assert(p->current_token && "Parser_cmd | current token is Null");

	Command *c = (Command *) malloc(sizeof(Command));
	if (! c) handle_error("Parser_cmd | malloc error");
	Command_init(c);

	// item (at least one item is required)
	int ret = Parser_item(p, c);
	if (ret == -1) return (Command_free(c), free(c), -1);
	// item not found -> parser error
	if (ret == 0) return (Parser_error(p), Command_free(c), free(c), -1);

	// {item}
	while (ret == 1) { // while there is an item
		ret = Parser_item(p, c);
		if (ret == -1) return (Command_free(c), free(c), -1);
	}
	// append cmd to pipeline
	List_insert(&p->pipeline, p->pipeline.last, &c->list);
	return (0);
}

/*
	0 -> ok
	-1 -> error

	pipeline := cmd {T_PIPE cmd};
*/
int Parser_pipeline(Parser *p, ListHead *token_list) {
	assert(p && "Parser_pipeline | parser is Null");
	assert(token_list && "Parser_pipeline | current token is Null");

	p->current_token = (Token *) token_list->first;

	// cmd (at leat one cmd is required)
	int ret = Parser_cmd(p);
	if (ret == -1) return (-1);
	// {T_PIPE cmd}
	while (p->current_token->type != T_NONE) {
		// T_PIPE
		if (p->current_token->type != T_PIPE)
			return (Parser_error(p), -1);
		p->current_token = (Token *) p->current_token->list.next; // next

		// cmd
		ret = Parser_cmd(p);
		if (ret == -1) return (-1);
		if (p->pipeline.size > MAX_CMDS)
			return (error(1, "Parser: maximum number of commands exceeded"), -1);
	}
	return (0);
}

