#include <execinfo.h>
#include <dlfcn.h>
#include <cxxabi.h>

#include "misc.h"

#include "backtrace.h"

using namespace std;

static string demangle(const string& name) {
    int status;
    char* buf = __cxxabiv1::__cxa_demangle(name.c_str(), 0, 0, &status);
    if (!buf) {
        return name;
    }
    string r(buf);
    free(buf);
    return r;
}

backtrace_holder::backtrace_holder() {
    fill();
}

void backtrace_holder::fill() {
    size_ = backtrace(pointers_, SIZE);
}

void backtrace_holder::init_hr() const {
    if (!human_readable_) {
        human_readable_.reset(new vector<string>());
        for (unsigned i = 0; i < size_; ++i) {
            Dl_info dl;
            string name;
            if (dladdr(pointers_[i], &dl) && dl.dli_sname) {
                name = demangle(dl.dli_sname);
            } else {
                name = "??";
            }
            string r = name + "+" + to_str(std::abs((long) pointers_[i] - (long) dl.dli_saddr));
            human_readable_->push_back(r);
        }
    }
}

void backtrace_holder::print(ostream& os) const {
    init_hr();
    for (unsigned i = 0; i < size_; ++i) {
        os << (*human_readable_).at(i) << endl;
    }
}

#include "test.h"

void some_useless_function(ostream& os) {
    backtrace_holder().print(os);
}

void test_print_backtrace() {
    stringstream ss;
    some_useless_function(ss);
    ua(contains(ss.str(), "some_useless_function"));
    ua(contains(ss.str(), "test_print_backtrace"));
}
