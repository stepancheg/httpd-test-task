#ifndef _TESTS_H_INCLUDED
#define _TESTS_H_INCLUDED

#include <functional>
#include <string>
#include <vector>
#include <iostream>

#include "misc.h"

typedef std::function<void ()> test_case_function;

/// very simple unittest library
class test_suite {
    struct test_case {
        std::string name_;
        test_case_function function_;
        test_case(const std::string& name, const test_case_function function):
            name_(name), function_(function)
        { }
    };
    std::vector<test_case> test_cases_;
public:
    void add_test(const std::string& name, const test_case_function& function) {
        test_cases_.push_back(test_case(name, function));
    }

    int run(int argc, const char* argv[]) {
        for (test_case& tc : test_cases_) {
            std::cerr << tc.name_ << " ..." << std::endl;
            tc.function_();
            std::cerr << tc.name_ << " is good" << std::endl;
        }
        return 0;
    }
};

#endif /* _TESTS_H_INCLUDED */
