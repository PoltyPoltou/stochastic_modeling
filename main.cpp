#include "coin/OsiClpSolverInterface.hpp"
#include "csv.h"
#include "dir_separator.h"

#include <iostream>
void testCsv(std::string data_dir) {
    csv::CsvFileStream csv_file(data_dir + "test.csv");
    csv_file.open();
    csv::skip_line(csv_file);
    std::vector<double> vector(csv::get_line(csv_file));
    assert(vector[0] == 10);
    assert(vector[1] == 10);
    assert(vector[2] == 8);
}

int main(int argc, char const *argv[]) {
    std::string arg1(argv[0]);
    int last_dir_sep(arg1.find_last_of(DIR_SEP));
    std::string data_dir(arg1.substr(0, last_dir_sep) + DIR_SEP + ".." + DIR_SEP
                         + "data" + DIR_SEP);

    OsiSolverInterface *test = new OsiClpSolverInterface;
    delete test;

    testCsv(std::string(data_dir));
    return 0;
}
