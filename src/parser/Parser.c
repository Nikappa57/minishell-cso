#include "Minishell.h"

void Parser_init(ListHead *p) {
	List_init(p);
}

void Parser_clear(ListHead *p) {
	if (!p->size || !p->first) {
		return;
	}

	ListItem* aux = p->first;
	while (aux) {
		Command* c_item = (Command*)(aux);
		assert(c_item && "Parser_clear | invalid token cast");
		aux = aux->next;
		Command_free(c_item);
		free(c_item);
	}
	List_init(p);
}