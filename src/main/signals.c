# include "signals.h"

static void ignore_sig(int sig) {
	struct sigaction sa = {0};

	sa.sa_handler = SIG_IGN;
	sigaction(sig, &sa, NULL);
}

// newline and update exit code
static void	sigint_handler(int sig) {
	(void)sig;
	write(STDOUT_FILENO, "\n", 1);
	// reset buffer and input line (0 => no redisplay)
	rl_replace_line("", 0);
	rl_on_new_line();
	rl_redisplay();
	g_exit_code = 130;
}

void set_shell_signals() {
	// Ctrl + C
	struct sigaction sa = {0};
	sa.sa_handler = sigint_handler;
	sigaction(SIGINT, &sa, NULL);

	// Ctrl + '\'
	ignore_sig(SIGQUIT);
	// Ctrl + Z
	ignore_sig(SIGTSTP);
	// read and write
	ignore_sig(SIGTTIN);
	ignore_sig(SIGTTOU);
}

void set_shell_signals_exe() {
	// Ctrl + C
	ignore_sig(SIGINT);
	// Ctrl + '\'
	ignore_sig(SIGQUIT);
	// Ctrl + Z
	ignore_sig(SIGTSTP);

	// read and write
	ignore_sig(SIGTTIN);
	ignore_sig(SIGTTOU);
}

// reset signals
void set_child_signals() {
	struct sigaction sa = {0};

	// set default handler
	sa.sa_handler=SIG_DFL;
	// Ctrl + C
	sigaction(SIGINT, &sa, NULL);
	// Ctrl + '\'
	sigaction(SIGQUIT, &sa, NULL);
	// Ctrl + Z
	sigaction(SIGTSTP, &sa, NULL);
	// read and write
	sigaction(SIGTTIN, &sa, NULL);
	sigaction(SIGTTOU, &sa, NULL);
}


