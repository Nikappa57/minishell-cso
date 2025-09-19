# include "signals.h"

static void ignore_sig(int sig) {
	struct sigaction sa = {0};

	sa.sa_handler = SIG_IGN;
	sigaction(sig, &sa, NULL);
}

void set_shell_signals() {
	// Ctrl + C
	ignore_sig(SIGINT);
	// Ctrl + '\'
	ignore_sig(SIGQUIT);
	// Ctrl + Z
	ignore_sig(SIGTSTP);
	// background read
	ignore_sig(SIGTTIN);
	// background write
	ignore_sig(SIGTTOU);
}


void set_child_signals() {
	struct sigaction sa={0};

	sa.sa_handler=SIG_DFL;
	// Ctrl + C
	sigaction(SIGINT, &sa, NULL);
	// Ctrl + '\'
	sigaction(SIGQUIT, &sa, NULL);
	// Ctrl + Z
	sigaction(SIGTSTP, &sa, NULL);
	// background read
	sigaction(SIGTTIN, &sa, NULL);
	// background write
	sigaction(SIGTTOU, &sa, NULL);
}


