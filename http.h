#ifndef _HTTP_H_INCLUDED
#define _HTTP_H_INCLUDED

#include <string>


struct http_request {
    bool complete_ = false;
    bool valid_ = false;
    std::string uri_;
};

http_request&& parse_http_request(const std::string& data);

#endif /* _HTTP_H_INCLUDED */

/* vim: set ts=4 sw=4 et: */
