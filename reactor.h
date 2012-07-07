#ifndef _REACTOR_H_INCLUDED
#define _REACTOR_H_INCLUDED

#include <memory>
#include <iostream>

#include "poller.h"
#include "misc.h"
#include "fd.h"

class reactor;
class reactor_descriptor;

// callback
class reactor_handler {
public:
    // called once right after registration
    virtual void initialize(reactor_descriptor* descriptor) = 0;
    // actual callback
    virtual void process_event() = 0;
};

class reactor_descriptor : noncopyable {
public:
    struct state;
    std::unique_ptr<state> state_;

    // fd associated with this descriptor
    const class fd& fd();
    // change poller mask
    void change_mask(int mask);
    // unregister this descriptor
    void remove();
};

// simple and incomplete implementation of the reactor pattern
// http://en.wikipedia.org/wiki/Reactor_pattern
class reactor : noncopyable {
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
