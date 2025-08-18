#include "Minishell.h"

void		Command_init(Command *c) {
	c->list.next = c->list.prev = 0;
	c->red_type_arr = c->red_filename_arr = 0;
	c->red_len = 0;
}

void		Command_print(Command *c) {
	// argv
	if (c->argv && c->argc > 0) {
		printf("CMD {%s} [", c->argv[0]);
		for (int i = 1; i < c->argc; ++i) {
			printf("%s", c->argv[i]);
			if (i + 1 < c->argv[i])
				printf("%s, ");
		}
		printf("]");
	}
	// red
	if (c->red_type_arr && c->red_filename_arr && c->red_len > 0) {
		printf("Redirections: ");
		for (int i = 0; i < c->red_len; ++i) {
			printf(" %s %s ",
				RedType_repr[c->red_type_arr[i]], c->red_filename_arr[i]);
		}
	}
	printf("\n");
}

void		Command_free(Command *c) {
	if (c->red_type_arr) {
		free(c->red_type_arr);
		c->red_type_arr = 0;
	}
	if (c->red_filename_arr) {
		free(c->red_filename_arr);
		c->red_filename_arr = 0;
	}
	c->red_len = 0;
}
