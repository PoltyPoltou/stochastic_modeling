#include "test.h"
#include "coin/CoinPackedMatrix.hpp"
#include "coin/CoinPackedVector.hpp"
#include "coin/OsiCpxSolverInterface.hpp"
#include "coin_util.h"
#include "csv.h"
#include "data.h"
#include "lp.h"
#include "probleme.h"
#include "stochastic.h"
#include <iostream>
#include <map>
#include <sstream>

void testall(std::string data_dir) {
    testCsv(data_dir);
    testCplex();
    testLpInterface();
    testReadRouteCsv(data_dir);
    testLoadDataLp(data_dir);
    testStochastic(data_dir);
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
    std::cout << "---CSV passed---" << std::endl;
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
    std::cout << "---CPLEX passed---" << std::endl;
}
void testLpInterface() {
    OsiCpxSolverInterface real_solver;
    lp::LinearProblem interface(real_solver);
    OsiSolverInterface &problem = interface.get_solver_interface();
    int x1, x2;
    interface.add_var(x1, -1);
    interface.add_var(x2, -1);
    int c1 = interface.add_constraint(0, 3);
    int c2 = interface.add_constraint(0, 3);
    interface.set_coef(c1, x1, 1);
    interface.set_coef(c2, x1, 2);
    interface.set_coef(c1, x2, 2);
    interface.set_coef(c2, x2, 1);
    interface.load_problem();
    interface.get_solver_interface().initialSolve();
    assert(problem.getObjValue() == -2);
    assert(problem.getColSolution()[0] == 1);
    assert(problem.getColSolution()[1] == 1);
    std::cout << "---lpInterface passed---" << std::endl;
}

void testReadRouteCsv(std::string data_dir) {
    Probleme pb(26460, 0.15, Livraison(1, 13, 1));
    assert(Probleme::commandes_set.size()
           == 24 * (Probleme::delai_max - Probleme::delai_min + 1)
                  * (Probleme::articles_max));

    load_std_itineraires(pb, data_dir);
    assert(pb.get_vec_itineraires().size() == 11);
    // testing first line of csv
    assert(pb.get_vec_itineraires()[0].get_cutoff() == 24);
    assert(pb.get_vec_itineraires()[0].get_delai() == 1);
    assert(pb.get_vec_itineraires()[0].get_prix_livraison() == 7.85);
    assert(pb.get_vec_itineraires()[0].get_prix()["PFS"] == 7.85);
    assert(!pb.get_vec_itineraires()[0].is_possible(0, 1)); // 0
    assert(!pb.get_vec_itineraires()[0].is_possible(1, 2)); // i
    assert(pb.get_vec_itineraires()[0].is_possible(1, 1));  // n

    std::map<int, double> delta_volu_map(
        load_delta_livraison_volu(pb, data_dir));
    // -1,-1 line should be skipped and the number of keys must be 5 - 1
    assert(delta_volu_map.size() == 4);
    // testing second line : 4.19,41.83,37.64
    assert(delta_volu_map.at(419) == 37.64);
    assert(delta_volu_map.at(419) == 41.83 - 4.19);

    load_livraison_pfs_volu(pb, data_dir);
    assert(pb.get_vec_livraison_volu_pfs().size() == 2);
    load_livraison_mag_volu(pb, data_dir);
    assert(pb.get_vec_livraison_volu_mag().size() == 2);

    Itineraire i1(1, 2, false, {false, false, true}, {{"PFS", 1}, {"Mag", 2}});
    Itineraire i2(i1);
    assert(i2.get_prix()["PFS"] == 1);
    i1.get_prix()["PFS"] = 3;
    assert(i1.get_prix()["PFS"] == 3);
    assert(i2.get_prix()["PFS"] == 1);

    generate_livraison_volu(pb, data_dir, delta_volu_map);
    assert(pb.get_vec_itineraires().size() == 45);

    generate_demand(pb);
    double std(0), volu(0);
    double q_std(0), q_volu(0);
    for (CommandeType cmd : Probleme::commandes_set) {
        std += pb.get_demande()[cmd][0];
        volu += pb.get_demande()[cmd][1];
        q_std += pb.getc_quantite(cmd, false);
        q_volu += pb.getc_quantite(cmd, true);
    }
    assert(std == volu);
    assert(abs(std - 1) < 0.001);
    assert(abs(q_std + q_volu - pb.get_nb_cmd()) < 0.001);
    assert(abs(q_volu - pb.get_ratio_volu() * pb.get_nb_cmd()) < 0.001);

    std::stringstream stream;
    for (Itineraire itin : pb.get_vec_itineraires()) {
        stream << itin << " " << get_prix_total_itineraire(pb, itin, 0, 5)
               << " " << get_prix_total_itineraire(pb, itin, 3, 5) << " "
               << get_prix_total_itineraire(pb, itin, 5, 5) << std::endl;
    }
    stream << pb.getc_nb_articles(false) << " " << pb.getc_nb_articles(true)
           << std::endl;
    std::cout << "---gen_data_from_csv passed---" << std::endl;
}

void testLoadDataLp(std::string data_dir) {
    Probleme pb(26460, 0.15, Livraison(1, 13, 1));
    read_and_gen_data_from_csv(pb, data_dir);

    OsiCpxSolverInterface solver_interface;
    lp::LinearProblem lin_pb(solver_interface);
    lin_pb.load_data_in_lp(pb);
    lin_pb.load_problem();
    lin_pb.get_solver_interface().initialSolve();
    if (abs(507090 - lin_pb.get_solver_interface().getObjValue()) >= 1) {
        std::cout << lp::get_str_solution(pb, lin_pb);
    }
    assert(abs(507090 - lin_pb.get_solver_interface().getObjValue()) < 1);
    std::cout << "---load_data_in_lp w/o stock var passed---" << std::endl;

    solver_interface = OsiCpxSolverInterface();
    lp::LpDecatWithStock lin_pb_stock_var(solver_interface);
    lin_pb_stock_var.load_data_in_lp(pb);
    lin_pb_stock_var.load_problem();
    lin_pb_stock_var.get_solver_interface().initialSolve();
    if (abs(457711 - lin_pb_stock_var.get_solver_interface().getObjValue())
        >= 1) {
        std::cout << lp::get_str_solution(pb, lin_pb_stock_var);
    }
    assert(abs(457711 - lin_pb_stock_var.get_solver_interface().getObjValue())
           < 1);
    std::cout << "---load_data_in_lp w/ stock var passed---" << std::endl;
}

void testStochastic(std::string data_dir) {
    OsiCpxSolverInterface solver;
    lp::LpDecatWithStock main_lp(stochastic_problem(data_dir, solver));
    Probleme pb(26460, 0.15, Livraison(1, 13, 1));
    read_and_gen_data_from_csv(pb, data_dir);
    main_lp.load_problem();
    main_lp.get_solver_interface().initialSolve();
    std::cout << lp::get_str_solution(pb, main_lp);
}