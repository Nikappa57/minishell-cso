# include "common.h"

unsigned char	g_exit_code = EXIT_SUCCESS;
bool			g_alive = true;

const int		OP_LEN[] = 			{1, 1, 1, 2, 2};
const char*		TokenType_repr[] =	{"T_PIPE", "T_RED_IN", "T_RED_OUT", "T_RED_OUT_APP",
										"T_HEREDOC", "T_WORD", "T_NONE"};
const char*		TokenType_str[] =	{"|", "<", ">", ">>",
										"<<", "word", "newline"};
const char*		RedType_repr[] =	{"R_IN", "R_OUT", "R_APP", "R_HD"};
const char*		RedType_str[] =		{"<", ">", ">>", "<<"};
const char*		JobState_str[] =	{"RUNNING", "STOPPED", "DONE"};