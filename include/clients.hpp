#ifndef _CLIENTS_HPP_
#define _CLIENTS_HPP_

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

#define BUF_SIZE 1024

enum socktype { TCP, UDP };

struct strm {
	int fd;
	socktype type;

	char *buf;
	size_t buf_size;

	size_t opos;
	size_t olength;
};

int sock_init(struct strm *s, size_t sze, socktype type);

#ifdef __cplusplus
}
#endif

#endif /* _CLIENTS_HPP_ */
