#include "Minishell.h"

// $...
static void expander_key(char **str, char **result) {

	if (!str || !*str || **str != '$')
		return ;
	++(*str); // skip $

	// '$'
	if (! **str) {
		char_append(result, '$');
		return ;
	}
	// $?
	if (**str == '?') {
		char code_str[32];
		snprintf(code_str, sizeof(code_str),"%d", g_exit_code);
		str_append(result, code_str);
		++*str; // skip ?
		return ;
	}
	// $<nbr> (invalid key)
	if (isdigit(**str)) {
		++*str; // skip number
		return ; // nothing
	}
	// get key [A-Za-z_][A-Za-z0-9_]*
	int len = 0;
	while (isalnum((unsigned char)(*str)[len]) || (*str)[len] == '_')
		len++;
	// no key
	if (len == 0) {
		char_append(result, '$'); // just a $
		return ;
	}
	// copy key
	char *key = (char *) malloc(len + 1);
	assert(key && "expander_key | malloc error");
	strncpy(key, *str, len);
	key[len] = 0;
	// get val
	const char *val = getenv(key);
	str_append(result, val);
	free(key);
	(*str) += len; // skip key
}	

// MARK_SQ ... MARK_SQ
static void copy_until_sq(char **str, char **result) {
	if (!str || !*str || **str != MARK_SQ)
		return ;
	++*str; // skip MARK_SQ
	char *next_sq = strchr(*str, MARK_SQ);
	assert(next_sq && "copy_until_sq | unclosed quotes"); // check in lexer
	
	*next_sq = 0;
	str_append(result, *str);
	*str = next_sq + 1; // skip quote
}

static void expander_str(char **pstr) {
	char *str = *pstr;
	char *result = 0;

	if (!str || !*str) return ;

	// expand ~ if is at the start of str
	if (*str == '~') {
		const char *home = getenv("HOME");
		if (home)
			str_append(&result, home);
		else
			char_append(&result, '~');
		++str; // skip ~
	}

	while (*str) {
		if (*str == MARK_DQ) {
			++str; // skip MARK_DQ
		}
		// single quotes
		else if (*str == MARK_SQ) {
			copy_until_sq(&str, &result); // side effect
		}
		// key
		else if (*str == '$') {
			expander_key(&str, &result); // side effect
		}
		else {
			char_append(&result, *str);
			++str; // skip char
		}
	}
	assert(result && "expander_str | result is Null");
	*pstr = result;
}

void expander_pipeline(ListHead *pipeline) {
	
	for (ListItem* c_aux = pipeline->first; c_aux; c_aux = c_aux->next) {
		Command* c_item = (Command *) (c_aux);
		assert(c_item && "Parser_clear | invalid token cast");
		
		// expand cmd args
		for (int i = 0; i < c_item->argc; i++)
			expander_str(&c_item->argv[i]);

		// expand cmd redirections
		for (ListItem* r_aux = c_item->redirections.first; r_aux; r_aux = r_aux->next) {
			Redirection *r_item = (Redirection *) (r_aux);
			expander_str(&r_item->filename);
		}
	}
}