#ifndef _SERVER_H_INCLUDED
#define _SERVER_H_INCLUDED

#include <memory>

#include "fd.h"


class http_server {
public:
    struct state;
    std::unique_ptr<state> state_;
public:
    // http server relies on externally created listen fd
    http_server(fd& fd);
    ~http_server();
};

#endif /* _SERVER_H_INCLUDED */

/* vim: set ts=4 sw=4 et: */
