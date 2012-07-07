#ifndef _EXCEPTION_H_INCLUDED
#define _EXCEPTION_H_INCLUDED

#include <exception>
#include <string>

#include "misc.h"
#include "backtrace.h"

// my exception that remembers stack trace
class exception {
public:
    backtrace_holder backtrace_;
private:
    std::string message_;
public:
    exception(const std::string& message)
        : message_(message)
    {
    }
    virtual const char* what() const throw() {
        return message_.c_str();
    }
};

#endif /* _EXCEPTION_H_INCLUDED */

/* vim: set ts=4 sw=4 et: */
