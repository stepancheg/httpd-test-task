#ifndef _FD_H_INCLUDED
#define _FD_H_INCLUDED


#include <memory>

#include "misc.h"
#include "backtrace.h"


// non-copyable file descriptor holder
class fd_owner : noncopyable {
    int fd_;
public:
    fd_owner(int fd): fd_(fd) {
        proper_assert(fd >= 0);
    }
    ~fd_owner();
    // unsafe operation: do not close it
    operator int() const {
        return fd_;
    }
};

// reference-counter fd holder
class fd {
    std::shared_ptr<fd_owner> fd_;
public:
    fd() {
    }
    explicit fd(int fd): fd_(new fd_owner(fd)) {
    }
    operator int() const {
        proper_assert(!!fd_);
        return *fd_;
    }
};

#endif /* _FD_H_INCLUDED */

/* vim: set ts=4 sw=4 et: */
