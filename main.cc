#include <iostream>
#include <thread>

#include <sys/socket.h>
#include <netinet/in.h>

#include "server.h"
#include "misc.h"
#include "fd.h"
#include "ptr.h"
#include "unix.h"

using namespace std;

// open listening socket
static fd listen() {
    fd fd_ = socket_x(PF_INET, SOCK_STREAM, 0);
    
    int yes = 1;
    setsockopt_x(fd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    
    union {
        sockaddr_in in_addr;
        sockaddr addr;
    };
    memset(&in_addr, 0, sizeof(in_addr));
    in_addr.sin_len = sizeof(in_addr);
    int port = 8877;
    in_addr.sin_port = htons(port);
    bind_x(fd_, &addr, sizeof(in_addr));
    listen_x(fd_);
    cerr << "listening on port " << port << endl;
    return fd_;
}

// run singlethreaded server with given descriptor
static void thread_proc(fd* fd) {
    try {
        http_server server_(*fd);
    } catch (...) {
        cerr << current_exception_information();
        exit(2);
    }
}

int main() {
    try {
        // shared descriptor for multiple single-threaded servers
        fd listen_fd = listen();
    
        vector<pthread_t> threads;
        for (int i = 0; i < 2; ++i) {
            pthread_t t;
            pthread_create_x(&t, 0, (void *(*)(void *)) thread_proc, &listen_fd);
            threads.push_back(t);
        }
        // unused currently, because proper server shutdown is not implemented
        for (auto thr : threads) {
            pthread_join_x(thr, 0);
        }
    } catch (...) {
        cerr << current_exception_information();
        exit(1);
    }
    cerr << "$" << endl;
    return 0;
}


// vim: set ts=4 sw=4 et:
