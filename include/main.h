# pragma once

# include "common.h"
# include "Lexer.h"
# include "Parser.h"
# include "env.h"
# include "Executor.h"
# include "redirections.h"
# include "signals.h"

void	print_start();
char*	get_prompt_string();
void	init_readline_completion();