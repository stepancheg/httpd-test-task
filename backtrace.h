#ifndef _BACKTRACE_H_INCLUDED
#define _BACKTRACE_H_INCLUDED

#include <string>
#include <iostream>
#include <vector>
#include <array>

#include "misc.h"
#include "types.h"
#include "ptr.h"

class backtrace_holder /* TODO: noncopyable */ {
    static const int SIZE = 32;

    void* pointers_[SIZE];
    unsigned size_ = 0;
    mutable unique_ptr<std::vector<std::string>> human_readable_;
public:
    backtrace_holder();
    void print(std::ostream& = std::cerr) const;
private:
    void init_hr() const;
    void fill();
};

inline void print_backtrace() {
    backtrace_holder().print();
}

#endif /* _BACKTRACE_H_INCLUDED */

// vim: set ts=4 sw=4 et:
