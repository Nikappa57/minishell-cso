# pragma once

# include "common.h"

typedef enum {
	R_IN,	// same number of TokenType
	R_OUT,
	R_APP,
	R_HD,
}	RedType;

extern const char* RedType_repr[];
extern const char* RedType_str[];

typedef struct {
	ListItem	list;
	RedType		type;
	char		*filename;
	int			hdoc_fd;
}	Redirection;


typedef struct {
	ListItem	list;
	char		**argv;	// argv[0] = cmd_name, argv[1:] = args, argv[argc] = None
	int			argc;
	// All redirection file will be created, only the last one used
	ListHead	redirections;
	// fd
	int			fdin;
	int			fdout;
}	Command;

void		Command_init(Command *c);
void		Command_print(Command *c);
void		Command_free(Command *c);
void		Command_add_arg(Command *c, const char *s);
void		Command_add_redirection(Command *c, RedType type, const char *s);

void		Command_set_fdin(Command *c, int fd);
void		Command_set_fdout(Command *c, int fd);
