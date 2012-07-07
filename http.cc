#include <regex>

#include "misc.h"

#include "http.h"


using namespace std;


http_request&& parse_http_request(const std::string& req) {
    http_request r;
    
    // TODO: better parser, faster parser, HTTP/1.0 and tons of features
    
    regex re("GET (/.*) HTTP/1.[01][^]*\r\n\r\n.*");
    
    smatch m;
    
    bool ok = regex_match(req, m, re);
    r.valid_ = ok;
    r.complete_ = contains(req, "\r\n\r\n");
    if (ok) {
        r.uri_ = m[1];
    }
    
    return move(r);
}


#include "test.h"


void test_parse_http_request_incomplete() {
    http_request r = parse_http_request("GET / HTTP");
    ua(!r.complete_);
}

void test_parse_http_request_invalid() {
    http_request r = parse_http_request("bla bla\r\nbla\r\n\r\n");
    ua(!r.valid_);
    ua(r.complete_);
}

void test_parse_http_request_valid() {
    http_request r = parse_http_request("GET /hello/world HTTP/1.1\r\nHost: localhost\r\n\r\n");
    ua(r.valid_);
    ua(r.complete_);
    ua_eq("/hello/world", r.uri_);
}

void test_parse_http_request_from_wget() {
    const char* rs =
        "GET /status HTTP/1.0\r\n"
        "User-Agent: Wget/1.12 (darwin10.7.0)\r\n"
        "Accept: */*\r\n"
        "Host: localhost:8877\r\n"
        "Connection: Keep-Alive\r\n"
        "\r\n";
    http_request r = parse_http_request(rs);
    ua(r.valid_);
    ua(r.complete_);
    ua_eq("/status", r.uri_);
}


// vim: set ts=4 sw=4 et:
