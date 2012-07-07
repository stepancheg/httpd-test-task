#ifndef _MISC_H_INCLUDED
#define _MISC_H_INCLUDED

#include <string>
#include <sstream>
#include <iostream>

#ifdef __clang__
#define h_override override
#else
#define h_override
#endif


// print nice exception info, with stack trace if available
std::string current_exception_information();

bool contains(const std::string& thiz, const std::string substring);

// unconditional assert
#define proper_assert(cond) \
    do { \
        if (!(cond)) { \
            std::cerr << "assertion failed: " << #cond << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
            print_backtrace(); \
            abort(); \
        } \
    } while (false)



// from boost
class noncopyable {
protected:
    noncopyable() {}
    ~noncopyable() {}
private:  // emphasize the following members are private
    noncopyable(const noncopyable&);
    const noncopyable& operator=(const noncopyable&);
};

#endif /* _MISC_H_INCLUDED */
