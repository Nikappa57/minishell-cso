# include <readline/readline.h>
# include <readline/history.h>
# include <dirent.h>
# include "builtin.h"


// generator function: called repeatedly for each match
static char* command_generator(const char *text, int state) {
	static int	idx;
	static int	len;
	static char	*path_env;
	static char	*current_env;
	static DIR	*current_dir;

	// first call: initialize
	if (!state) {
		idx = 0;
		len = strlen(text);
		// save PATH
		path_env = getenv("PATH");
		if (path_env) path_env = strdup(path_env);
		current_env = strtok(path_env, ":");
		current_dir = 0;
	}

	const char *str;
	while ((str = BUILTINS[idx].name)) {
		++idx;
		if (! strncmp(str, text, len))
			return (strdup(str));
	}

	while (current_env) {
		// open a new directory
		if (!current_dir) {
			current_dir = opendir(current_env);
			// NULL if it could not be opened
			if (! current_dir) {
				current_env = strtok(0, ":");
				continue;
			}
		}

		struct dirent *entry;
		while ((entry = readdir(current_dir))) {
			if (! strncmp(entry->d_name, text, len))
				return strdup(entry->d_name);
		}

		closedir(current_dir);
		current_dir = 0;
		current_env = strtok(0, ":");
	}

	if (path_env) {
		free(path_env);
		path_env = 0;
	}

	return (0);
}

static char** minishell_completion(const char *text, int start, int end) {
	(void)end;

	// if at start of line: complete commands and filenames
	if (start == 0)
		return rl_completion_matches(text, command_generator);
	// else: complete filenames (default)
	return (0);
}

// initialize readline completion
void init_readline_completion() {
	rl_attempted_completion_function = minishell_completion;

	// man:	words which have more than one possible
	// 		completion cause the matches to be listed
	rl_variable_bind("show-all-if-ambiguous", "on");

	// man:	menu completion displays the common prefix
	//		of the list of possible completions
	rl_variable_bind("menu-complete-display-prefix", "on");

	// cycle through options on TAB
	rl_bind_key('\t', rl_menu_complete);
}