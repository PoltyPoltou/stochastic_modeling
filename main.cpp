#include "coin/OsiClpSolverInterface.hpp"
#include "csv.h"
#include "dir_separator.h"

#include <iostream>
void testCsv(std::string arg1) {
    int last_dir_sep(arg1.find_last_of(DIR_SEP));
    std::string dir_exec(arg1.substr(0, last_dir_sep));
    csv::CsvFileStream csv(dir_exec + DIR_SEP + ".." + DIR_SEP + "data"
                           + DIR_SEP + "test.csv");
    csv.open();
    csv::skip_line(csv);
    std::vector<double> vector(csv::get_line(csv));
    assert(vector[0] == 10);
    assert(vector[1] == 10);
    assert(vector[2] == 8);
}

int main(int argc, char const *argv[]) {
    OsiSolverInterface *test = new OsiClpSolverInterface;
    delete test;
    testCsv(std::string(argv[0]));
    return 0;
}
