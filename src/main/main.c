# include "main.h"

// main loop
static void shell_loop() {
	char		*line;
	ListHead	lexer;
	Parser		parser;
	Executor	executor;

	Lexer_init(&lexer);
	Executor_init(&executor);
	print_start();
	while (g_alive) {
		// ignore signals in shell process
		set_shell_signals();

		// update jobs info
		Executor_update_jobs(&executor);

		// readline
		line = readline(get_prompt_string());
		if (!line) { // EOF
			fprintf(stderr, "exit\n");
			break ;
		}
		if (*line) add_history(line); // save line in history

		// update jobs info
		Executor_update_jobs(&executor);

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

		// executor
		Executor_exe(&executor, &parser.pipeline, line);

		// cleanup
		Parser_clear(&parser);
		Lexer_clear(&lexer);
		if (line) free(line);
		line = 0;

		if (DEBUG) Executor_print(&executor);
	}
	// cleanup
	Parser_clear(&parser);
	Lexer_clear(&lexer);
	if (line) free(line);
	Executor_clear(&executor);
	return ;
}

int main(int argc, char **argv) {
	(void)argv;
	if (argc != 1)
	{
		fprintf(stderr, "Arguments not allowed\n");
		return (EXIT_ERROR);
	}
	// init env (update shell level)
	env_init();
	if (DEBUG) env_print();

	// start main loop
	shell_loop();

	return (g_exit_code);
}