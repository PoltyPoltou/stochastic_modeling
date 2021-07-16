#include "coin/CoinPackedMatrix.hpp"
#include "coin/CoinPackedVector.hpp"
#include "coin/OsiCpxSolverInterface.hpp"
#include "csv.h"
#include "dir_separator.h"

#include <iostream>
#include <vector>

void testCsv(std::string data_dir) {
    csv::CsvFileStream csv_file(data_dir + "test.csv");
    csv_file.open();
    csv::skip_line(csv_file);
    std::vector<double> vector(csv::get_line(csv_file));
    assert(vector[0] == 10);
    assert(vector[1] == 10);
    assert(vector[2] == 8);
}

CoinPackedVector std_to_coin_vector(std::vector<double> &vec) {
    CoinPackedVector coin_vec;
    for (int i = 0; i < vec.size(); i++) {
        coin_vec.insert(i, vec[i]);
    }
    return coin_vec;
}

void testCplex() {
    OsiSolverInterface *problem = new OsiCpxSolverInterface;

    std::vector<double> obj({-1, -1}), col_lb({0, 0}), col_ub({100, 100}),
        row_lb({0, 0}), row_ub({3, 3}), row1({1, 2}), row2({2, 1});
    CoinPackedMatrix matrice;
    matrice.setDimensions(0, 2);
    matrice.appendRow(std_to_coin_vector(row1));
    matrice.appendRow(std_to_coin_vector(row2));
    problem->loadProblem(matrice, col_lb.data(), col_ub.data(), obj.data(),
                         row_lb.data(), row_ub.data());
    problem->initialSolve();
    assert(problem->getObjValue() == -2);
    assert(problem->getColSolution()[0] == 1);
    assert(problem->getColSolution()[1] == 1);
    delete problem;
}

int main(int argc, char const *argv[]) {
    std::string arg1(argv[0]);
    int last_dir_sep(arg1.find_last_of(DIR_SEP));
    std::string data_dir(arg1.substr(0, last_dir_sep) + DIR_SEP + ".." + DIR_SEP
                         + "data" + DIR_SEP);

    testCplex();
    testCsv(std::string(data_dir));
    return 0;
}
