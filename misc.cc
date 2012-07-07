#include <sstream>

#include "exception.h"

#include "misc.h"

using std::stringstream;
using std::endl;
using std::string;

string current_exception_information() {
    stringstream ss;
    try {
        throw;
    } catch (const exception& e) {
        ss << e.what() << endl;
        e.backtrace_.print(ss);
    } catch (const std::exception& e) {
        ss << e.what() << endl;
    } catch (...) {
        ss << "unknown exception" << endl;
    }
    return ss.str();
}

bool contains(const string& thiz, const std::string substring) {
    return thiz.find(substring) != string::npos;
}


#include "test.h"

void test_current_exception_information() {
    try {
        throw std::exception();
    } catch (...) {
        string s = current_exception_information();
        ua(s.length() > 0);
    }
}

void test_contains() {
    ua(contains(string("abc"), "bc"));
    ua(!contains(string("abc"), "cb"));
}

// vim: set ts=4 sw=4 et:
