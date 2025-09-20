# include "common.h"
# include "Lexer.h"
# include "Parser.h"
# include "env.h"
# include "Executor.h"
# include "redirections.h"
# include "signals.h"

// main loop
static void shell_loop() {
	char		*line;
	ListHead	lexer;
	Parser		parser;
	Executor	executer;

	Lexer_init(&lexer);
	Executor_init(&executer);
	while (g_alive) {
		// ignore signals in shell process
		set_shell_signals();

		// readline
		line = readline(">> ");
		if (!line) { // EOF
			fprintf(stderr, "exit\n");
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
			continue;
		}
		if (DEBUG) Parser_print(&parser);

		// expander
		expander_pipeline(&parser.pipeline);
		if (DEBUG) printf("--- After expander ---\n");
		if (DEBUG) Parser_print(&parser);

		// here document
		heredoc_pipeline(&parser.pipeline);
		if (DEBUG) printf("--- After hdoc ---\n");
		if (DEBUG) Parser_print(&parser);

		// executer
		Executor_exe(&executer, &parser.pipeline);

		// cleanup
		Parser_clear(&parser);
		Lexer_clear(&lexer);
		if (line) free(line);
		line = 0;
	}
	// cleanup
	Parser_clear(&parser);
	Lexer_clear(&lexer);
	if (line) free(line);
	Executor_clear(&executer);
	return ;
}

int main(int argc, char **argv) {
	(void)argv;
	if (argc != 1)
	{
		fprintf(stderr, "Arguments not allowed\n");
		return (EXIT_ERROR);
	}
	// init env (updare shell level)
	env_init();
	if (DEBUG) env_print();

	// start main loop
	shell_loop();

	return (g_exit_code);
}