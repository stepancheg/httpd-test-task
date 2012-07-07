#ifndef _POLLER_H_INCLUDED
#define _POLLER_H_INCLUDED

#include <memory>
#include <vector>
#include <iostream>

enum {
    POLLER_READ   = 1 << 0,
    POLLER_WRITE  = 1 << 1,
};

struct poll_result {
    int fd_;
    void* tag_;
};

class poller {
    struct state;
    std::unique_ptr<state> state_;
public:
    poller();
    ~poller();
    void add_fd(int fd, int mask, void* tag);
    void remove_fd(int fd);
    // change mask of specified descriptor (may be zero)
    void change_fd(int fd, int new_mask);
    
    // poll once
    void poll(std::vector<poll_result>* result);
    
    // print poll state
    void debug_dump(std::ostream& os);
};

#endif /* _POLLER_H_INCLUDED */

// vim: set ts=4 sw=4 et:
