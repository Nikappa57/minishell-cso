#include "Minishell.h"

void lexer_test() {
	char * good_test_str[] = {
		"ls",
		"ls | cat",
		"ls| cat",
		"ls|cat",
		"ls|cat|cat|cat | cat| cat| cat| cat|cat           |             cat",
		"echo ciao>>out >> out >>out >>>>>> >> >> out >>             out >>out",
		"echo ciao>out > out >out> out > > out >      out    > out",
		"cat < in<in< in< in                           < in",
		"cat << EOF <<EOF<<EOF <<EOF <<EOF<< EOF        << EOF << << <<<< <<",
		"<<<>>> <> <><>><<>>",
		"a b c d e \"- - - -\" \' d d d \' f g h",
		"\"\" a\"b\"c a\'b\'c ",
		"\" \' exp \' \"",
		"\' \" noexp \" \'",
		"\\\'exp\\\'  \\\" token \\\"",
		"\a \b \\ \\\\"
	};

	char * err_test_str[] = {
		"\"",
		"\'",
		"\"\"\"",
		"\\",
		"abc cbdbc \\",
	};

	
	ListHead lexer;

	Lexer_init(&lexer);
	printf("Good test:\n");
	for (size_t i = 0; i < sizeof(good_test_str) / sizeof(char *); i++) {
		printf(">> \"%s\"\n", good_test_str[i]);
		int ret = Lexer_line(&lexer, good_test_str[i]);
		if (ret == -1)
			printf("Lexer line error, skip.\n");
		else 
			Lexer_print(&lexer);
		Lexer_clear(&lexer);
	}
	Lexer_clear(&lexer);

	printf("Error test:\n");
	for (size_t i = 0; i < sizeof(err_test_str) / sizeof(char *); i++) {
		printf(">> \"%s\"\n", err_test_str[i]);
		int ret = Lexer_line(&lexer, err_test_str[i]);
		if (ret == -1)
			printf("Lexer line error, skip.\n");
		else {
			Lexer_print(&lexer);
			assert(false && "Should raise an error");
		}
		Lexer_clear(&lexer);
	}
	Lexer_clear(&lexer);
}