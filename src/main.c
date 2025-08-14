#include "minishell.h"

unsigned char	exit_code = EXIT_SUCCESS;
bool			alive = true;

static void shell_loop()
{
	char *line;

	while (alive)
	{
		line = readline(">> ");
		if (!line)
		{
			fprintf(stderr, EXIT_MSG);
			break ;
		}
		if (*line)
			add_history(line);
	}
	return ;
}

int main(int argc, char **argv, char **envp)
{
	(void)argv;
	(void)envp;
	if (argc != 1)
	{
		fprintf(stderr, ERR_ARG_NOT_ALLOW);
		return (EXIT_ERROR);
	}
	shell_loop();
	return (exit_code);
}