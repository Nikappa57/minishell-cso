#include "Minishell.h"

static int heredoc_fd(char *end) {
	int		fd[2];
	char	*line;

	int ret = pipe(fd);
	assert((ret == 0) && "heredoc_fd | pipe error");
	while (42)
	{
		line = readline("> ");
		if (!line)
		{
			fprintf(stderr, "error: warning: here-document at line 1 delimited by end-of-file (wanted `%s')", end);
			break ;
		}
		if (strcmp(line, end) == 0)
		{
			free(line);
			break ;
		}
		ret = write(fd[1], line, strlen(line));
		assert(ret != -1 && "heredoc_fd | write error");
		ret = write(fd[1], "\n", 1);
		assert(ret != -1 && "heredoc_fd | write error");
		free(line);
	}
	ret = close(fd[1]);
	assert(ret == 0 && "heredoc_fd | close error");
	return (fd[0]);
}

void heredoc_pipeline(ListHead *pipeline) {
	for (ListItem* c_aux = pipeline->first; c_aux; c_aux = c_aux->next) {
		Command* c_item = (Command *) (c_aux);
		assert(c_item && "heredoc_pipeline | invalid token cast");

		// expand cmd redirections
		for (ListItem* r_aux = c_item->redirections.first; r_aux; r_aux = r_aux->next) {
			Redirection *r_item = (Redirection *) (r_aux);
			assert(r_item && "expander_pipeline | invalid redirection cast");
			
			if (r_item->type == R_HD) {
				int fd = heredoc_fd(r_item->filename);
				Command_set_fdin(c_item, fd);
			}
		}
	}
}