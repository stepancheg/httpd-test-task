#ifndef _TEST_H_INCLUDED
#define _TEST_H_INCLUDED

#include "misc.h"
#include "backtrace.h"

/// unittest assert
#define ua(cond) do { \
        if (!(cond)) { \
            std::cerr << "assertion " #cond " failed, " \
                __FILE__ ":" << __LINE__ << " " << __func__ << "()" << std::endl; \
            print_backtrace(); \
            abort(); \
        } \
    } while (0)

/// unittest comparison
#define ua_op(expected, op, got) do { \
        if (!((expected) op (got))) { \
            std::cerr << "assertion " #expected " " #op " " #got " failed: !(" << (expected) << " " #op " " << (got) << ")" \
                ", at " __FILE__ ":" << __LINE__ << " " << __func__ << "()" << std::endl; \
            print_backtrace(); \
            abort(); \
        } \
    } while (0)

/// unittest equals
#define ua_eq(expected, got) ua_op(expected, ==, got)    

#endif /* _TEST_H_INCLUDED */
