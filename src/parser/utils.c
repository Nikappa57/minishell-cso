# include "Command.h"
# include "Token.h"

RedType token_to_red(TokenType t) {
	RedType rd;
	switch (t) {
		case T_RED_IN:		rd = R_IN;  break;
		case T_RED_OUT:		rd = R_OUT; break;
		case T_RED_OUT_APP:	rd = R_APP; break;
		case T_HEREDOC:		rd = R_HD;break;
		default:
			assert("token_to_red | invalid token " && TokenType_repr[t]);
	}
	return rd;
}