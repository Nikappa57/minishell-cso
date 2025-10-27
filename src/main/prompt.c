# include "common.h"

void print_start() {
	printf(BLUE "Minishell CSO" RESET);
	char *level = getenv("SHLVL");
	if (level) {
		printf(CYAN " (level %s)" RESET, level);
	}
	printf("\n");
}

char* get_prompt_string() {
	static char prompt[512];
	
	// folder
	char *folder = "?";
	char *path = getenv("PWD");
	if (path) {
		char *slash = strrchr(path, '/');
		folder = slash ? slash + 1 : path;
		if (!*folder) folder = "?";
	}
	
	// git branch
	char branch[128] = {0};
	FILE *fp = popen("git branch --show-current 2>/dev/null", "r");
	if (fp) {
		if (fgets(branch, sizeof(branch), fp)) {
			char *nl = strchr(branch, '\n');
			if (nl) *nl = '\0';
		}
		pclose(fp);
	}
	
	// build prompt
	snprintf(prompt, sizeof(prompt), 
		BLUE "%s" RESET "%s%s%s" "%s >> " RESET,
		folder,
		*branch ? CYAN " git:(" : "",
		*branch ? branch : "",
		*branch ? ")" RESET : "",
		g_exit_code == 0 ? GREEN : RED);
	
	return prompt;
}