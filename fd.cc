#include <unistd.h>

#include "fd.h"

fd_owner::~fd_owner() {
    if (fd_ > 0) {
        // TODO: check errors
        ::close(fd_);
    }
}

// vim: set ts=4 sw=4 et:
