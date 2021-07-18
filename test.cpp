#include "test.h"
#include "coin/CoinPackedMatrix.hpp"
#include "coin/CoinPackedVector.hpp"
#include "coin/OsiCpxSolverInterface.hpp"
#include "coin_util.h"
#include "csv.h"
#include "data.h"
#include "probleme.h"
#include <iostream>

void testall(std::string data_dir) {
    testCsv(data_dir);
    testCplex();
    testReadRouteCsv(data_dir);
}

void testCsv(std::string data_dir) {
    csv::CsvFileStream csv_file(data_dir + "test.csv");
    csv_file.open();
    csv::skip_line(csv_file);
    std::vector<double> vector;
    csv::get_line(csv_file, vector);
    assert(vector[0] == 10);
    assert(vector[1] == 10);
    assert(vector[2] == 8);
}

void testCplex() {
    OsiCpxSolverInterface real_solver;
    OsiSolverInterface &problem(real_solver);
    std::vector<double> obj({-1, -1}), col_lb({0, 0}), col_ub({100, 100}),
        row_lb({0, 0}), row_ub({3, 3}), row1({1, 2}), row2({2, 1});
    CoinPackedMatrix matrice;
    matrice.setDimensions(0, 2);
    matrice.appendRow(std_to_coin_vector(row1));
    matrice.appendRow(std_to_coin_vector(row2));
    problem.loadProblem(matrice, col_lb.data(), col_ub.data(), obj.data(),
                        row_lb.data(), row_ub.data());
    problem.initialSolve();
    assert(problem.getObjValue() == -2);
    assert(problem.getColSolution()[0] == 1);
    assert(problem.getColSolution()[1] == 1);
}

void testReadRouteCsv(std::string data_dir) {
    Probleme pb(100, 0.15, Livraison(0, 1, 13));
    read_and_gen_data_from_csv(pb, data_dir);
}