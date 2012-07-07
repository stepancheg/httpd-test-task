#include <iostream>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <string.h>

#include "reactor.h"
#include "fd.h"
#include "unix.h"
#include "http.h"

#include "server.h"


using namespace std;


enum connection_state {
    CONN_READING,
    CONN_SENDING_RESPONSE,
};

struct connection_handler;
struct read_file_handler;

struct read_file_handler : reactor_handler {
    http_server::state* http_server_;
    reactor_descriptor* reactor_descriptor_;
    connection_handler* connection_handler_;
    
    read_file_handler(http_server::state* http_server, connection_handler* connection_handler)
        : http_server_(http_server)
        , reactor_descriptor_(nullptr)
        , connection_handler_(connection_handler)
    {
    }
    
    void initialize(reactor_descriptor* reactor_descriptor) h_override {
        reactor_descriptor_ = reactor_descriptor;
        reactor_descriptor->change_mask(0);
    }
    
    void process_event() h_override;
    
};


struct connection_handler : reactor_handler {
    http_server::state* http_server_;
    reactor_descriptor* reactor_descriptor_;
    vector<char> request_data;
    connection_state connection_state_;
    
    // TODO: replace with some buffer
    string response_chunk_;
    size_t response_chunk_pos_;
    
    shared_ptr<read_file_handler> read_file_handler_;
    
    connection_handler(http_server::state* http_server)
        : http_server_(http_server)
        , reactor_descriptor_(nullptr)
        , connection_state_(CONN_READING)
    {
    }
    
    void initialize(reactor_descriptor* reactor_descriptor) h_override {
        reactor_descriptor_ = reactor_descriptor;
        reactor_descriptor->change_mask(POLLER_READ);
    }
    
    void send_error(int code, const char* message) {
        //cerr << "sending error " << code << " " << message << endl;
        stringstream response;
        response << "HTTP/1.1 " << code << " Name\r\n"; // TODO: proper status name
        response << "Content-type: text/plain\r\n";
        response << "\r\n";
        response << code << " " << message << "\r\n";
        response_chunk_ = response.str();
        response_chunk_pos_ = 0;
        reactor_descriptor_->change_mask(POLLER_WRITE);
        connection_state_ = CONN_SENDING_RESPONSE;
    }
    
    void process_read();
    void process_write();
    
    void process_event() h_override {
        try {
            if (connection_state_ == CONN_READING) {
                process_read();
            } else {
                process_write();
            }
        } catch (...) {
            cerr << current_exception_information();
            kill_me_softly();
        }
    }
    
    void kill_me_softly() {
        reactor_descriptor_->remove();
        if (!!read_file_handler_) {
            read_file_handler_->reactor_descriptor_->remove();
        }
    }
};

struct accept_handler : reactor_handler {
    http_server::state* http_server_;
    reactor_descriptor* reactor_descriptor_;
    
    accept_handler(http_server::state* http_server)
        : http_server_(http_server)
        , reactor_descriptor_(nullptr)
    {
    }

    void initialize(reactor_descriptor* reactor_descriptor) h_override {
        reactor_descriptor_ = reactor_descriptor;
        reactor_descriptor->change_mask(POLLER_READ|POLLER_WRITE);
    }
    
    void process_event() h_override;
};


struct http_server::state {
    reactor reactor_;
    fd fd_;
    
    state(fd& fd) {
        fd_ = fd;
        
        shared_ptr<reactor_handler> accept_handler_(new accept_handler(this));
        reactor_.add_descriptor(fd_, accept_handler_);
        
        cerr << "entering reactor loop" << endl;
        reactor_.run();
    }
    
    void debug_dump(ostream& os) {
        os << "listen fd " << int(fd_) << endl;
        os << "reactor:" << endl;
        reactor_.debug_dump(os);
    }
};

http_server::http_server(fd& fd)
    : state_(new state(fd))
{
}

http_server::~http_server() {
}


void accept_handler::process_event() {
    union {
        sockaddr_in in_addr;
        sockaddr_in6 in6_addr;
        sockaddr sockaddr_addr;
    } addr;
    socklen_t addr_len = sizeof(addr);
    int fd_ = ::accept(reactor_descriptor_->fd(), &addr.sockaddr_addr, &addr_len);
    cerr << "accepted" << endl;
    proper_assert(fd_ >= 0);
    
    shared_ptr<reactor_handler> connection_handler_(new connection_handler(http_server_));
    http_server_->reactor_.add_descriptor(fd(fd_), connection_handler_);
}

void connection_handler::process_read() {
    size_t chunk = 0x1000;
    size_t old_size = request_data.size();
    // TODO: fills with zeros
    request_data.resize(old_size + chunk);
    int c = read_x(reactor_descriptor_->fd(), &request_data[old_size], chunk);
    request_data.resize(old_size + c);
    if (c == 0) {
        //cerr << "eof, read " << request_data.size() << endl;
        reactor_descriptor_->change_mask(0);
    }
    
    string request_string(request_data.size() > 0 ? &request_data[0] : "", request_data.size());
    http_request request = parse_http_request(request_string);
    
    //cerr << request_string;
    
    if (!request.complete_) {
        if (c == 0) {
            send_error(400, "invalid request");
        }
        return;
    }
    
    if (!request.valid_) {
        send_error(500, "invalid request");
        return;
    }
    
    if (contains(request.uri_, "..")) {
        send_error(404, "do not use .. in path");
        return;
    }
    
    if (request.uri_ == "/status") {
        stringstream status_response;
        status_response << "HTTP/1.1 200 OK\r\n";
        status_response << "Content-type: text/plain; charset=utf-8\r\n";
        status_response << "\r\n";
        http_server_->debug_dump(status_response);
        response_chunk_ = status_response.str();
        response_chunk_pos_ = 0;
        reactor_descriptor_->change_mask(POLLER_WRITE);
        connection_state_ = CONN_SENDING_RESPONSE;
        return;
    }
    
    string path = string() + "." + request.uri_;
    // TODO: async open
    int file_rv = ::open(path.c_str(), O_RDONLY);
    if (file_rv < 0) {
        send_error(404, strerror(errno));
        return;
    }
    
    fd file_fd(file_rv);
    
    struct stat stat_;
    fstat_x(file_fd, &stat_);
    
    if ((stat_.st_mode & S_IFREG) == 0) {
        send_error(404, "not regular file");
        return;
    }
    
    cerr << "sending file " << path << endl;
    
    read_file_handler_ = shared_ptr<read_file_handler>(new read_file_handler(http_server_, this));
    shared_ptr<reactor_handler> read_file_reactor_handler_ = read_file_handler_;
    http_server_->reactor_.add_descriptor(file_fd, read_file_reactor_handler_);
    stringstream response;
    response << "HTTP/1.1 200 OK\r\n";
    response << "Content-type: application/octet-stream\r\n";
    response << "\r\n";
    response_chunk_ = response.str();
    response_chunk_pos_ = 0;
    reactor_descriptor_->change_mask(POLLER_WRITE);
    connection_state_ = CONN_SENDING_RESPONSE;
}

void connection_handler::process_write() {
    proper_assert(response_chunk_pos_ <= response_chunk_.size());
    
    const void* buf = response_chunk_.c_str() + response_chunk_pos_;
    size_t buflen = response_chunk_.size() - response_chunk_pos_;
    size_t written = write_x(reactor_descriptor_->fd(), buf, buflen);
    response_chunk_pos_ += written;
    
    proper_assert(response_chunk_pos_ <= response_chunk_.size());
    
    if (response_chunk_pos_ != response_chunk_.size()) {
        return;
    }
    
    reactor_descriptor_->change_mask(0);
    
    if (!!read_file_handler_) {
        read_file_handler_->reactor_descriptor_->change_mask(POLLER_READ);
    } else {
        cerr << "response sent" << endl;
        kill_me_softly();
    }
}

void read_file_handler::process_event() {
    try {
        char buf[0x1000];
        size_t readen = read_x(reactor_descriptor_->fd(), buf, sizeof(buf));
        if (readen == 0) {
            cerr << "file response sent" << endl;
            connection_handler_->kill_me_softly();
        } else {
            connection_handler_->response_chunk_ = string(buf, readen);
            connection_handler_->response_chunk_pos_ = 0;
            connection_handler_->reactor_descriptor_->change_mask(POLLER_WRITE);
            reactor_descriptor_->change_mask(0);
        }
    } catch (...) {
        cerr << current_exception_information();
        connection_handler_->kill_me_softly();
    }
}

// vim: set ts=4 sw=4 et:
