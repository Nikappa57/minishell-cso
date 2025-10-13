# include <readline/readline.h>
# include <readline/history.h>
# include "builtin.h"


// generator function: called repeatedly for each match
static char* command_generator(const char *text, int state) {
	static int list_index, len;

	// first call: initialize
	if (!state) {
		list_index = 0;
		len = strlen(text);
	}

	const char *str;
	while ((str = BUILTINS[list_index++].name)) {
		if (! strncmp(str, text, len))
			return strdup(str);
	}

	return (0);
}

static char** minishell_completion(const char *text, int start, int end) {
	(void)end;

	// if at start of line: complete commands
	if (start == 0) {
		// disable default filename completion
		rl_attempted_completion_over = 1;
		return rl_completion_matches(text, command_generator);
	}
	// else: complete filenames (default)
	return (0);
}

// initialize readline completion
void init_readline_completion() {
	rl_attempted_completion_function = minishell_completion;
}