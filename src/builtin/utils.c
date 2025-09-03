# include "builtin.h"

bool	str_isdigit(const char *s)
{
	if (! s || ! *s) return (false);

	int i = 0;
	if ((s[i] == '+' || s[i] == '-') && s[i + 1])
		i++;
	while (s[i])
		if (!isdigit(s[i++]))
			return (false);
	return (true);
}