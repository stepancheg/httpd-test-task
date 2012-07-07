#ifndef _FD_H_INCLUDED
#define _FD_H_INCLUDED

#include "misc.h"
#include "ptr.h"
#include "backtrace.h"

class fd_owner : noncopyable {
    int fd_;
public:
    fd_owner(int fd): fd_(fd) {
        proper_assert(fd >= 0);
    }
    ~fd_owner();
    operator int() const {
        return fd_;
    }
};

class fd {
    shared_ptr<fd_owner> fd_;
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
