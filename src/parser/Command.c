#include "Command.h"

void Command_init(Command *c) {
	c->list.next = c->list.prev = 0;
	c->argc = 0;
	c->argv = (char **) malloc(sizeof(char *));
	if (! c->argv) handle_error("Command_init | malloc error");
	c->argv[0] = 0;
	List_init(&c->redirections);
	// fd
	c->fdin = -1;
	c->fdout = -1;
}

void Command_print(Command *c) {
	// argv
	if (c->argv && c->argc > 0) {
		printf("CMD '%s' {", c->argv[0]);
		for (int i = 1; i < c->argc; ++i) {
			printf("'%s'", c->argv[i]);
			if (i + 1 < c->argc)
				printf(", ");
		}
		printf("} ");
	}
	// red
	for (ListItem *item = c->redirections.first;
			item != NULL; item = item->next) {
		Redirection* r_item = (Redirection*)(item);
		assert(r_item && "Command_print | invalid cast");
		printf("'%s' '%s' ", RedType_str[r_item->type], r_item->filename);
	}
	printf("\n");
	// fd
	printf("FD: in %d out %d\n", c->fdin, c->fdout);
}

void Command_free(Command *c) {
	// redirections
	if (c->redirections.size && c->redirections.first) {
		ListItem* aux = c->redirections.first;
		while (aux) {
			Redirection* t_item = (Redirection*)(aux);
			assert(t_item && "Command_free | invalid token cast");
			aux = aux->next;
			free(t_item->filename);
			free(t_item);
		}
		List_init(&c->redirections);
	}

	// argv
	if (c->argv) {
		for (int i = 0; i < c->argc; ++i) {
			if (c->argv[i]) free(c->argv[i]);
		}
		free(c->argv);
		c->argv = 0;
		c->argc = 0;
	}

	// fd
	int ret;
	if (c->fdin > 2) {
		ret = close(c->fdin);
		if (ret != 0) handle_error("Command_free | close fdin error");
		c->fdin = -1;
	}
	if (c->fdout > 2) {
		ret = close(c->fdout);
		if (ret != 0) handle_error("Command_free | close fdout error");
		c->fdin = -1;
	}
}

void Command_add_arg(Command *c, const char *s) {
	c->argv = (char **) realloc(c->argv, (++(c->argc) + 1) * (sizeof(char *)));
	if (! c->argv) handle_error("Command_add_arg | realloc error");
	c->argv[c->argc - 1] = strdup(s);
	if (! c->argv[c->argc - 1]) handle_error("Command_add_arg | strdup error");
	c->argv[c->argc] = 0;
}

void Command_add_redirection(Command *c, RedType type, const char *s) {
	Redirection *r = (Redirection *) malloc(sizeof(Redirection));
	if (! r) handle_error("Command_add_redirection | malloc error");
	r->list.next = r->list.prev = 0;
	r->type = type;
	r->filename = strdup(s);
	r->hdoc_fd = -1;
	List_insert(&c->redirections, c->redirections.last, &r->list);
}

int Command_set_fdin(Command *c, int fd) {
	if (c->fdin > 2) {
		int ret = close(c->fdin);
		if (ret == -1)
			return (error(1, "close error: %s", strerror(errno)), -1);
	}
	c->fdin = fd;
	return (0);
}

int Command_set_fdout(Command *c, int fd) {
	if (c->fdout > 2) {
		int ret = close(c->fdout);
		if (ret == -1)
			return (error(1, "close error: %s", strerror(errno)), -1);
	}
	c->fdout = fd;
	return (0);
}

int Command_dup2(Command *c) {
	int ret;
	if (c->fdin > 2) {
		ret = dup2(c->fdin, STDIN_FILENO);
		if (ret == -1)
			return (error(1, "Command_dup2 | dup2 error (in): %s", strerror(errno)), -1);
		ret = close(c->fdin);
		c->fdin = -1;
		if (ret == -1)
			return (error(1, "Command_dup2 | close error (in): %s", strerror(errno)), -1);
	}
	if (c->fdout > 2) {
		ret = dup2(c->fdout, STDOUT_FILENO);
		if (ret == -1)
			return (error(1, "Command_dup2 | dup2 error (out): %s", strerror(errno)), -1);
		ret = close(c->fdout);
		c->fdout = -1;
		if (ret == -1)
			return (error(1, "Command_dup2 | close error (out): %s", strerror(errno)), -1);
	}
	return (0);
}
