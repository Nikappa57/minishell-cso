#include "Minishell.h"

// global vars

unsigned char	g_exit_code = EXIT_SUCCESS;
bool			g_alive = true;

const int		OP_LEN[] = 			{1, 1, 1, 2, 2};
const char*		TokenType_repr[] =	{"T_PIPE", "T_RED_IN", "T_RED_OUT", "T_RED_OUT_APP",
										"T_HEREDOC", "T_WORD", "T_NONE"};
const char*		TokenType_str[] =	{"|", "<", ">", ">>",
										"<<", "word", "newline"};
const char*		RedType_repr[] =	{"R_IN", "R_OUT", "R_APP", "R_HD"};
const char*		RedType_str[] =		{"<", ">", ">>", "<<"};

// main loop
static void shell_loop() {
	char		*line;
	ListHead	lexer;
	Parser		parser;

	Lexer_init(&lexer);
	while (g_alive) {
		// readline
		line = readline(">> ");
		if (!line) { // EOF
			fprintf(stderr, EXIT_MSG);
			break ;
		}
		if (*line) add_history(line); // save line in history

		// lexer
		int ret = Lexer_line(&lexer, line);
		if (ret == -1) {
			if (DEBUG) printf("Lexer line error, skip.\n");
			Lexer_clear(&lexer);
			free(line);
			continue;
		}
		if (DEBUG) Lexer_print(&lexer);
		
		// parser
		Parser_init(&parser, &lexer);
		ret = Parser_pipeline(&parser);
		if (ret == -1) {
			if (DEBUG) printf("Parser line error, skip.\n");
			Lexer_clear(&lexer);
			Parser_clear(&parser);
			free(line);
			continue;
		}
		if (DEBUG) Parser_print(&parser);
		
		// expander
		expander_pipeline(&parser.cmd_list);
		if (DEBUG) printf("--- After expander ---\n");
		if (DEBUG) Parser_print(&parser);

		// here document
		heredoc_pipeline(&parser.cmd_list);
		if (DEBUG) printf("--- After hdoc ---\n");
		if (DEBUG) Parser_print(&parser);

		// cleanup
		Parser_clear(&parser);
		Lexer_clear(&lexer);
		free(line);
		line = 0;
	}
	Lexer_clear(&lexer);
	return ;
}

int main(int argc, char **argv) {
	(void)argv;
	if (argc != 1)
	{
		fprintf(stderr, ERR_ARG_NOT_ALLOW);
		return (EXIT_ERROR);
	}
	// init env (updare shell level)
	env_init();
	if (DEBUG) env_print();

	// start main loop
	shell_loop();

	return (g_exit_code);
}