#include "Minishell.h"

// global vars

unsigned char	g_exit_code = EXIT_SUCCESS;
bool			g_alive = true;

const int OP_LEN[] = {1, 1, 1, 2, 2};
const char* TokenType_repr[] = {"T_PIPE", "T_RED_IN", "T_RED_OUT", "T_RED_OUT_APP",
							"T_HEREDOC", "T_WORD", "T_NONE"};
const char* TokenType_str[] = {"|", "<", ">", ">>",
							"<<", "word", "newline"};
const char* RedType_repr[] = {"R_IN", "R_OUT", "R_APP", "R_HD"};
const char* RedType_str[] = {"<", ">", ">>", "<<"};

// main loop
static void shell_loop()
{
	char		*line;
	ListHead	lexer;
	Parser		parser;

	Lexer_init(&lexer);
	while (g_alive)
	{
		line = readline(">> ");
		if (!line)
		{
			fprintf(stderr, EXIT_MSG);
			break ;
		}
		// save line in history
		if (*line)
			add_history(line);
		
		// lexer
		int ret = Lexer_line(&lexer, line);
		if (ret == -1) {
			if (DEBUG) printf("Lexer line error, skip.\n");
			Lexer_clear(&lexer);
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
			continue;
		}
		if (DEBUG) Parser_print(&parser);
		Parser_clear(&parser);

		// clear lexer
		Lexer_clear(&lexer);
	}
	Lexer_clear(&lexer);
	Parser_clear(&parser);
	return ;
}

int main(int argc, char **argv)
{
	(void)argv;

	// lexer_test();
	// return 0;

	if (argc != 1)
	{
		fprintf(stderr, ERR_ARG_NOT_ALLOW);
		return (EXIT_ERROR);
	}
	// init env (shell level)
	env_init();
	if (DEBUG) env_print();
	// start main loop
	shell_loop();
	return (g_exit_code);
}