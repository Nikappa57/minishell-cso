# include "Minishell.h"

/* Check if is operation, skip operator and return operator token type */
TokenType check_operator(char *s)
{
	if (!s || !*s)
		return (T_NONE);
	if (strncmp(s, "<<", 2) == 0)
		return (T_HEREDOC);
	if (strncmp(s, ">>", 2) == 0)
		return (T_RED_OUT_APP);
	if (strncmp(s, ">", 1) == 0)
		return (T_RED_OUT);
	if (strncmp(s, "<", 1) == 0)
		return (T_RED_IN);
	if (strncmp(s, "|", 1) == 0)
		return (T_PIPE);
	return (T_NONE);
}

/* move pointer after whitespaces */
void skip_ws(char **s)
{
	while (isspace(**s))
		(*s)++;
}

bool eol(char *s) {
	return (!s || !*s || *s == '\n');
}

/* print error and set exit code */
void error(char *err, int code) {
	fprintf(stderr, "error: %s\n", err);
	g_exit_code = code;
}
