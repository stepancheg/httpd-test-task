#ifndef _HTTP_H_INCLUDED
#define _HTTP_H_INCLUDED

#include <string>


struct http_request {
    // request contains header
    bool complete_ = false;
    // request is valid
    bool valid_ = false;
    // request uri is request is valid and complete
    std::string uri_;
};

http_request&& parse_http_request(const std::string& data);

#endif /* _HTTP_H_INCLUDED */

/* vim: set ts=4 sw=4 et: */
