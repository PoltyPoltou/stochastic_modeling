
#include "test.h"
#include <iostream>
#include <vector>

int main(int argc, char const *argv[]) {
    std::string arg1(argv[0]);
    int last_dir_sep(arg1.find_last_of('/'));
    std::string data_dir(arg1.substr(0, last_dir_sep) + "/../data/");

    testall(data_dir);
    return 0;
}
