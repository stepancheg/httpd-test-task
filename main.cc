#include <iostream>

#include "server.h"
#include "misc.h"

using namespace std;

int main() {
    try {
        http_server server_;
    } catch (...) {
        cerr << current_exception_information();
        exit(1);
    }
    cerr << "$" << endl;
    return 0;
}


// vim: set ts=4 sw=4 et:
