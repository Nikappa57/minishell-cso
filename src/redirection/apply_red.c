# include "redirections.h"
# include "Command.h"

int redirections_apply(Command *c) {
	int ret = 0;

	// save cmd fdout and fdin
	for (ListItem *it = c->redirections.first; it; it = it->next) {

		Redirection *r = (Redirection*)it;
		assert(r && "apply_redirs_in_child | invalid cast");

		switch (r->type) {
			case R_IN:
				ret = open(r->filename, O_RDONLY);
				if (ret < 0)
					return (error(1, "%s : %s", r->filename, strerror(errno)), -1);
				ret = Command_set_fdin(c, ret);
				break;

			case R_OUT:
				ret = open(r->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
				if (ret < 0)
					return (error(1, "%s : %s", r->filename, strerror(errno)), -1);
				ret = Command_set_fdout(c, ret);
				break;

			case R_APP:
				ret = open(r->filename, O_WRONLY | O_CREAT| O_APPEND, 0644);
				if (ret < 0)
					return (error(1, "%s : %s", r->filename, strerror(errno)), -1);
				ret = Command_set_fdout(c, ret);
				break;

			case R_HD:
				assert(r->hdoc_fd > 2 && "apply_redirs_in_child | invalid hdoc fd");
				ret = Command_set_fdin(c, r->hdoc_fd);
				r->hdoc_fd = -1;
				break;
			default:
				assert(false && "apply_redirs_in_child | invalid redirection type");
		}
		// Command_set_fd* error
		if (ret == -1) return (-1);
	}
	
	// dup2
	ret = Command_dup2(c);
	return (ret);
}

