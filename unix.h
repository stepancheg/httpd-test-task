#ifndef _UNIX_H_INCLUDED
#define _UNIX_H_INCLUDED

#include <sys/socket.h>
#include <sys/stat.h>


#include "fd.h"

fd socket_x(int domain, int type, int protocol);
void bind_x(int fd, const sockaddr* addr, socklen_t address_len);
void listen_x(int fd, int backlog = 5);
size_t read_x(int fd, void* buf, size_t nbytes);
size_t write_x(int fd, const void* buf, size_t nbytes);
void fstat_x(int fd, struct stat* buf);
void setsockopt_x(int socket, int level, int option_name, const void *option_value, socklen_t option_len);

#endif /* _UNIX_H_INCLUDED */

/* vim: set ts=4 sw=4 et: */
