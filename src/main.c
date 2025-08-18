#include "Minishell.h"

unsigned char	g_exit_code = EXIT_SUCCESS;
bool			g_alive = true;

const int OP_LEN[] = {1, 1, 1, 2, 2};
const char* TokenType_repr[] = {"T_PIPE", "T_RED_IN", "T_RED_OUT", "T_RED_OUT_APP",
							"T_HEREDOC", "T_WORD", "T_NONE"};
const char* RedType_repr[] = {"R_IN", "R_OUT", "R_APP", "R_HD"};

static void shell_loop()
{
	char *line;
	ListHead lexer;

	Lexer_init(&lexer);
	while (g_alive)
	{
		line = readline(">> ");
		if (!line)
		{
			fprintf(stderr, EXIT_MSG);
			break ;
		}
		if (*line)
			add_history(line);

		int ret = Lexer_line(&lexer, line);
		if (ret == -1) {
			printf("Lexer line error, skip.\n");
			continue;
		}
		Lexer_print(&lexer);
		Lexer_clear(&lexer);
	}
	Lexer_clear(&lexer);
	return ;
}

int main(int argc, char **argv, char **envp)
{
	(void)argv;
	(void)envp;

	// lexer_test();
	// return 0;

	if (argc != 1)
	{
		fprintf(stderr, ERR_ARG_NOT_ALLOW);
		return (EXIT_ERROR);
	}
	shell_loop();
	return (g_exit_code);
}