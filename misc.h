#ifndef _MISC_H_INCLUDED
#define _MISC_H_INCLUDED

#include <string>
#include <sstream>
#include <iostream>

std::string current_exception_information();

template <typename T>
std::string to_str(const T& t) {
    std::stringstream ss;
    ss << t;
    return ss.str();
}

bool contains(const std::string& thiz, const std::string substring);

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
