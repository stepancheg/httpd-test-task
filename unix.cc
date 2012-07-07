#include <exception>
#include <string>

#include <sys/types.h>
#include <unistd.h>

#include "exception.h"

#include "unix.h"

using std::string;

static void check_rv(int rv, const char* what) {
    if (rv < 0) {
        string message = string() + what + " failed: " + strerror(errno);
        throw exception(message);
    }
}

fd socket_x(int domain, int type, int protocol) {
    int rv = socket(domain, type, protocol);
    check_rv(rv, "socket");
    return fd(rv);
}

void bind_x(int fd, const sockaddr* addr, socklen_t addr_len) {
    int rv = bind(fd, addr, addr_len);
    check_rv(rv, "bind");
}

void listen_x(int fd, int backlog) {
    int rv = listen(fd, backlog);
    check_rv(rv, "listen");
}

size_t read_x(int fd, void* buf, size_t nbytes) {
    int rv = read(fd, buf, nbytes);
    check_rv(rv, "read");
    return (size_t) rv;
}

size_t write_x(int fd, const void* buf, size_t nbytes) {
    int rv = write(fd, buf, nbytes);
    check_rv(rv, "write");
    return (size_t) rv;
}

void fstat_x(int fd, struct stat* buf) {
    int rv = fstat(fd, buf);
    check_rv(rv, "fstat");
}

void setsockopt_x(int socket, int level, int option_name, const void *option_value, socklen_t option_len) {
    int rv = setsockopt(socket, level, option_name, option_value, option_len);
    check_rv(rv, "setsockopt");
}

// vim: set ts=4 sw=4 et:
