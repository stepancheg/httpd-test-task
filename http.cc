#include "misc.h"
#include "backtrace.h"

#include "http.h"


using namespace std;


// TODO: very inefficient
struct simple_parser {
    const string input_;
    size_t pos_;
    
    simple_parser(const string& input)
        : input_(input)
        , pos_(0)
    {
    }
    
    bool looking_at_eof() {
        proper_assert(pos_ <= input_.size());
        return pos_ == input_.size();
    }
    
    void check_not_eof() {
        proper_assert(!looking_at_eof());
    }
    
    bool looking_at(const string& ahead) {
        return input_.substr(pos_, ahead.size()) == ahead;
    }
    
    char lookahead() {
        check_not_eof();
        return input_.at(pos_);
    }
    
    bool looking_at(char c) {
        return !looking_at_eof() && lookahead() == c;
    }
    
    bool looking_at_not(char c) {
        return !looking_at_eof() && lookahead() != c;
    }
    
    void consume(const string& ahead) {
        proper_assert(looking_at(ahead));
        pos_ += ahead.size();
    }
    
    void consume(char c) {
        return consume(string() + c);
    }
    
    bool consume_if_looking_at(const string& ahead) {
        bool r = looking_at(ahead);
        if (r) {
            consume(ahead);
        }
        return r;
    }
    
    char consume() {
        check_not_eof();
        char r = lookahead();
        ++pos_;
        return r;
    }
    
    string consume_until(char c) {
        stringstream r;
        while (looking_at_not(c)) {
            r << consume();
        }
        return r.str();
    }
};


http_request parse_http_request(const string& req) {
    http_request r;
    
    // TODO: better parser, faster parser, HTTP/1.0 and tons of features
    
    r.complete_ = contains(req, "\r\n\r\n");
    r.valid_ = false;
    
    auto p = simple_parser(req);
    
    if (!p.consume_if_looking_at("GET ")) {
        return move(r);
    }
    
    // TODO: exclude newlines
    string uri = p.consume_until(' ');
    if (p.looking_at_eof()) {
        return move(r);
    }
    
    p.consume(' ');
    
    // TODO: read protocol version
    // TODO: read headers
    
    r.uri_ = uri;
    r.valid_ = true;
    
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
