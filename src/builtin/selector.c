# include "builtin.h"

int placeholder(Command *c) {
	printf("*** CMD ***\n");
	Command_print(c);
	return (0);
}

static const builtin BUILTINS[] = {
	{"cd",		placeholder},
	{"export",	placeholder},
	{"unset",	placeholder},
	{"exit",	placeholder},
	{"echo",	placeholder},
	{"pwd",		placeholder},
	{0, 0}
};

builtin_fn find_builtin(Command *c){
	if (!c || c->argc == 0) return (0);
	for (int i=0; BUILTINS[i].name; ++i)
		if (strcmp(BUILTINS[i].name, c->argv[0]) == 0)
			return BUILTINS[i].fn;
	return (0);
}