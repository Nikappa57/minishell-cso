#include <Minishell.h>

// append s2 to s1
void str_append(char **dst, const char *src) {
	if (!dst) return;
	if (!src) src = "";
	size_t a = *dst ? strlen(*dst) : 0;
	size_t b = strlen(src);
	char *p = realloc(*dst, a + b + 1);
	assert(p && "str_append | realloc error");
	memcpy(p + a, src, b + 1);
	*dst = p;
}

// append c to s1
void char_append(char **s1, char c) {
	const char s2[] = {c, 0};
	str_append(s1, s2);
}
