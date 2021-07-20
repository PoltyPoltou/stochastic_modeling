#include "test.h"
#include "coin/CoinPackedMatrix.hpp"
#include "coin/CoinPackedVector.hpp"
#include "coin/OsiCpxSolverInterface.hpp"
#include "coin_util.h"
#include "csv.h"
#include "data.h"
#include "lp.h"
#include "probleme.h"
#include <iostream>
#include <map>
#include <sstream>

void testall(std::string data_dir) {
    testCsv(data_dir);
    testCplex();
    testLpInterface();
    testReadRouteCsv(data_dir);
    testLoadDataLp(data_dir);
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
    Probleme pb(100 * 24 * 3 * 8, 0.15, Livraison(0, 1, 13));
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
    for (CommandeType cmd : Probleme::commandes_set) {
        std += pb.get_demande()[cmd][0];
        volu += pb.get_demande()[cmd][1];
    }
    assert(std == volu);
    assert(std > 0.9999);
    assert(std < 1.0001);
    std::cout << "---gen_data_from_csv passed---" << std::endl;
}

void testLoadDataLp(std::string data_dir) {
    Probleme pb(26460, 0.15, Livraison(1, 13, 1));
    read_and_gen_data_from_csv(pb, data_dir);

    OsiCpxSolverInterface solver_interface;
    lp::LinearProblem lin_pb(solver_interface);

    lp::Commande_Variable_map cmd_var_map = lp::load_data_in_lp(pb, lin_pb);
    std::cout << "loaded in matrix" << std::endl;
    std::map<std::string, double> set_volu, set_std;
    std::map<std::string, double> preparation_costs(
        {{"Mag", 0}, {"PFS", 0}, {"CAR", 0}});
    lin_pb.load_problem();
    std::cout << "loaded in cplex" << std::endl;
    lin_pb.get_solver_interface().initialSolve();

    std::stringstream buffer;
    for (CommandeType cmd : Probleme::commandes_set) {
        for (lp::Variable v : cmd_var_map.at(cmd)[0]) {
            for (std::string lieu : LIEUX_VOLU) {
                preparation_costs[lieu] +=
                    pb.getc_quantite(cmd, false)
                    * lin_pb.get_solver_interface()
                          .getColSolution()[v.problem_idx]
                    * get_prix_prepa_itineraire(pb, v.route, v.i,
                                                cmd.get_nb_articles(), lieu);
            }
            if (lin_pb.get_solver_interface().getColSolution()[v.problem_idx]
                > 0.001) {
                buffer = std::stringstream();
                buffer << v.route << ", value : ";
                if (set_std.contains(buffer.str())) {
                    set_std[buffer.str()] +=
                        lin_pb.get_solver_interface()
                            .getColSolution()[v.problem_idx]
                        * pb.getc_quantite(cmd, false);
                } else {
                    set_std[buffer.str()] = lin_pb.get_solver_interface()
                                                .getColSolution()[v.problem_idx]
                                            * pb.getc_quantite(cmd, false);
                }
            }
        }
        for (lp::Variable v : cmd_var_map.at(cmd)[1]) {
            for (std::string lieu : LIEUX_VOLU) {
                preparation_costs[lieu] +=
                    pb.getc_quantite(cmd, true)
                    * lin_pb.get_solver_interface()
                          .getColSolution()[v.problem_idx]
                    * get_prix_prepa_itineraire(pb, v.route, v.i,
                                                cmd.get_nb_articles(), lieu);
            }
            if (lin_pb.get_solver_interface().getColSolution()[v.problem_idx]
                > 0.001) {
                buffer = std::stringstream();
                buffer << v.route << ", value : ";
                if (set_volu.contains(buffer.str())) {
                    set_volu[buffer.str()] +=
                        lin_pb.get_solver_interface()
                            .getColSolution()[v.problem_idx]
                        * pb.getc_quantite(cmd, true);
                } else {
                    set_volu[buffer.str()] =
                        lin_pb.get_solver_interface()
                            .getColSolution()[v.problem_idx]
                        * pb.getc_quantite(cmd, true);
                }
            }
        }
    }
    for (auto iter : set_std) {
        std::cout << iter.first << iter.second << std::endl;
    }
    std::cout << std::endl;

    for (auto iter : set_volu) {
        std::cout << iter.first << iter.second << std::endl;
    }
    for (auto iter : preparation_costs) {
        std::cout << iter.first << " : " << iter.second << std::endl;
    }
    std::cout << lin_pb.get_solver_interface().getObjValue() << std::endl;
}