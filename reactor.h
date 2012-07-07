#ifndef _REACTOR_H_INCLUDED
#define _REACTOR_H_INCLUDED

#include <memory>
#include <iostream>

#include "poller.h"
#include "misc.h"
#include "fd.h"

class reactor;
class reactor_descriptor;

class reactor_handler {
public:
    virtual void initialize(reactor_descriptor* descriptor) = 0;
    virtual void process_event() = 0;
};

class reactor_descriptor : noncopyable {
public:
    struct state;
    std::unique_ptr<state> state_;

    const fd& fd();
    void change_mask(int mask);
    void remove();
};

class reactor: noncopyable {
public:
    struct state;
    std::unique_ptr<state> state_;

    reactor();
    ~reactor();
    void add_descriptor(const fd& fd, std::shared_ptr<reactor_handler>& handler);
    void run();
    void debug_dump(std::ostream& os);
};

#endif /* _REACTOR_H_INCLUDED */

// vim: set ts=4 sw=4 et:
