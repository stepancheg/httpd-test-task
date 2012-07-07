#ifndef _SERVER_H_INCLUDED
#define _SERVER_H_INCLUDED

#include "ptr.h"

class http_server {
public:
    struct state;
    unique_ptr<state> state_;
public:
    http_server();
    ~http_server();
};

#endif /* _SERVER_H_INCLUDED */

/* vim: set ts=4 sw=4 et: */
