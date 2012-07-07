#ifndef _SERVER_H_INCLUDED
#define _SERVER_H_INCLUDED

#include "ptr.h"
#include "fd.h"

class http_server {
public:
    struct state;
    unique_ptr<state> state_;
public:
    // fd is listen fd
    http_server(fd& fd);
    ~http_server();
};

#endif /* _SERVER_H_INCLUDED */

/* vim: set ts=4 sw=4 et: */
