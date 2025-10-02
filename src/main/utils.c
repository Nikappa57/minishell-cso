# include "utils.h"

// append s2 to s1
void str_append(char **dst, const char *src) {
	if (!dst) return;
	if (!src) src = "";
	size_t a = *dst ? strlen(*dst) : 0;
	size_t b = strlen(src);
	char *p = realloc(*dst, a + b + 1);
	if (! p) handle_error("str_append | realloc error");
	memcpy(p + a, src, b + 1);
	*dst = p;
}

// append c to s1
void char_append(char **s1, char c) {
	const char s2[] = {c, 0};
	str_append(s1, s2);
}


bool str_isdigit(const char *s) {
	if (! s || ! *s) return (false);

	int i = 0;
	// skip sign
	if ((s[i] == '+' || s[i] == '-') && s[i + 1])
		i++;
	while (s[i])
		if (!isdigit(s[i++]))
			return (false);
	return (true);
}
