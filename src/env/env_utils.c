# include "env.h"

void env_init() {
	// update shell lvl
	const char *cur = getenv("SHLVL");
	long lvl = 0;
	if (cur) {
		char *end;
		lvl = strtol(cur, &end, 10);
		if (*end != 0) lvl = 0; // strtol error
	}
	if (lvl < 0 || lvl >= MAX_SHLVL)
		lvl = 0;
	++lvl;
	char buf[32];
	snprintf(buf, sizeof(buf), "%ld", lvl); // to str
	setenv("SHLVL", buf, 1);

	// set shell path
	char shell_path[PATH_MAX];
	// get absolute path of process binary
	ssize_t len = readlink("/proc/self/exe", shell_path, sizeof(shell_path) - 1);
	if (len == -1)
		perror("readlink");
	else {
		shell_path[len] = '\0';
		setenv("SHELL", shell_path, 1);
	}
}

// check if is a valid env ke y
// [A-Za-z_][A-Za-z0-9_]*
static bool env_is_valid_key(const char *s) {
	if (! s || ! (*s)) return (0);
	if (! (isalpha((unsigned char) *s) || *s == '_')) return (0);
	for (s++; *s; ++s) {
		if (! (isalnum((unsigned char) *s) || *s == '_'))
			return (0);
	}
	return (1);
}

void env_print() {
	if (DEBUG) printf("--- ENV print ---\n");
	for (char **e = environ; e && *e; ++e) puts(*e);
	if (DEBUG) printf("--- ENV print end ---\n");
}

int env_export(const char *name, const char *value) {
	if (! env_is_valid_key(name))
		return (error(1, "export: `%s': not a valid identifier", name), 1);
	return setenv(name, value ? value : "", /* overwrite= */ 1);
}
