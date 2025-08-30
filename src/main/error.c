# include "common.h"
#include <stdarg.h>

/* print custom error message and set exit code */
void error(int code, const char *str, ...) {
	va_list args;
	fprintf(stderr, "error: ");
	va_start(args, str);
	vfprintf(stderr, str, args);
	va_end(args);
	fprintf(stderr, "\n");
	g_exit_code = code;
}
