# include "builtin.h"

builtin_fn find_builtin(Command *c){
	if (!c || c->argc == 0) return (0);
	for (int i=0; BUILTINS[i].name; ++i)
		if (strcmp(BUILTINS[i].name, c->argv[0]) == 0)
			return BUILTINS[i].fn;
	return (0);
}