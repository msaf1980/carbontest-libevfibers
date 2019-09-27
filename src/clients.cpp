#include <clients.hpp>

#include <stdlib.h>

int sock_init(struct strm *s, size_t sze, socktype type) {
	if (sze > 0) {
		if ((s->buf = (char *) malloc(sze)) == NULL)
			return -1;
	} else {
		s->buf = NULL;
	}
	s->buf_size = sze;
	s->olength = 0;
	s->opos = 0;
	return 0;
}
