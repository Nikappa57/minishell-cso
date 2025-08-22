# include "Minishell.h"

// set shell level
void env_init() {
	const char *cur = getenv("SHLVL");
	long lvl = 0;
	if (cur) {
		char *end; lvl = strtol(cur, &end, 10);
		if (*end != '\0') lvl = 0; // strtol error
	}
	if (lvl < 0) lvl = 0;
	++lvl;
	char buf[32];
	snprintf(buf, sizeof(buf), "%ld", lvl); // to str
	setenv("SHLVL", buf, 1);
}

// check if is a valid env key
// [A-Za-z_][A-Za-z0-9_]*
static bool env_is_valid_key(const char *s) {
	if (!s || !(*s)) return (false);
	if (!(isalpha((unsigned char)*s) || *s=='_')) return (0);
	for (s++; *s; ++s)
		if (!(isalnum((unsigned char)*s) || *s=='_'))
			return (false);
	return (true);
}

static void env_error(const char *cmd, const char *arg) {
	fprintf(stderr, "%s: `%s': not a valid identifier\n", cmd, arg);
}

void env_print() {
	printf("--- ENV print ---\n");
	for (char **e = environ; e && *e; ++e) puts(*e);
	printf("--- ENV print end ---\n");
}

int env_export(const char *name, const char *value) {
	if (! env_is_valid_key(name)) return (env_error("export", name), -1);
	return setenv(name, value ? value : "", /* overwrite= */ 1);
}

int env_unset(const char *name) {
	if (! env_is_valid_key(name)) return (env_error("unset", name), -1);
	return unsetenv(name);
}
