#include "Minishell.h"

void Command_init(Command *c) {
	c->list.next = c->list.prev = 0;
	c->argc = 0;
	c->argv = (char **) malloc(sizeof(char *));
	assert(c->argv && "Command_init | malloc error");
	c->argv[0] = 0;
	List_init(&c->redirections);
}

void Command_print(Command *c) {
	// argv
	if (c->argv && c->argc > 0) {
		printf("CMD {%s} [", c->argv[0]);
		for (int i = 1; i < c->argc; ++i) {
			printf("%s", c->argv[i]);
			if (i + 1 < c->argc)
				printf(", ");
		}
		printf("]");
	}
	// red
	printf("RED: ");
	for (ListItem *item = c->redirections.first;
			item != NULL; item = item->next) {
		Redirection* r_item = (Redirection*)(item);
		assert(r_item && "Command_print | invalid cast");
		printf(" %s %s ", RedType_repr[r_item->type], r_item->filename);
	}
	printf("\n");
}

void Command_free(Command *c) {
	// redirections
	if (c->redirections.size && c->redirections.first) {
		ListItem* aux = c->redirections.first;
		while (aux) {
			Redirection* t_item = (Redirection*)(aux);
			assert(t_item && "Command_free | invalid token cast");
			aux = aux->next;
			free(t_item);
		}
		List_init(&c->redirections);
	}

	// argv
	if (c->argv) {
		for (int i = 0; i < c->argc + 1; i++) {
			free(c->argv[i]);
		}
		free(c->argv);
		c->argv = 0;
		c->argc = 0;
	}
}

void Command_add_arg(Command *c, const char *s) {
	c->argv = (char **) realloc(c->argv, (++c->argc + 1) * (sizeof(char *)));
	assert(c->argv && "Command_add_arg | realloc error");
	c->argv[c->argc - 1] = strdup(s);
	c->argv[c->argc] = 0;
}

void Command_add_redirection(Command *c, RedType type, const char *s) {
	Redirection *r = (Redirection *) malloc(sizeof(Redirection));
	assert(r && "Command_add_redirection | malloc error");
	r->list.next = r->list.prev = 0;
	r->type = type;
	r->filename = strdup(s);
	List_insert(&c->redirections, c->redirections.last, &r->list);
}