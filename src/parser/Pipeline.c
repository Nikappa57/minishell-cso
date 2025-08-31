# include "Parser.h"

void Pipeline_print(ListHead *p) {
	assert(p && "Pipeline_print | pipeline is Null");

	printf("Pipeline:\n");
	if (p->size && p->first) {
		ListItem* aux = p->first;
		int i = 0;
		while (aux) {
			Command* c_item = (Command*)(aux);
			assert(c_item && "Pipeline_print | invalid token cast");
			printf("[%d] ", i++);
			Command_print(c_item);
			aux = aux->next;
		}
	}
}

void Pipeline_clear(ListHead *p) {
	assert(p && "Pipeline_clear | p is Null");

	if (p->size && p->first) {
		ListItem* aux = p->first;
		while (aux) {
			Command* c_item = (Command*)(aux);
			assert(c_item && "Pipeline_clear | invalid token cast");
			aux = aux->next;
			Command_free(c_item);
			free(c_item);
		}
		List_init(p);
	}
}
