#include "stochastic.h"
#include "data.h"
#include <cmath>
#include <coin/OsiCuts.hpp>
#include <iostream>
#include <iterator>
inline double normal_probabilty(double x, double mean, double var) {
    return (std::erf((x - mean) / var) + 1) / 2;
}

lp::LpDecatScenarios stochastic_problem(std::string data_dir,
                                        OsiSolverInterface &solver_interface,
                                        ProblemeStochastique &pb_loaded) {
    lp::LpDecatScenarios main_lp;
    std::vector<double> scenarios({0.9, 1, 1.1});
    double width = 0.05;
    main_lp.create_stock_variables(
        pb_loaded.getc_nb_articles()
        * 0.1); // coût par 100% de distribution de stock
    main_lp.create_recours_var(
        50); // coût par article pour étendre le stock (backlog)
    for (double ratio : scenarios) {
        pb_loaded.set_nb_cmd_mesured(ratio * pb_loaded.get_nb_cmd());
        main_lp.create_variables(pb_loaded, 1 / scenarios.size());
        main_lp.fullfilment_constraint(pb_loaded);
        main_lp.stock_constraint(pb_loaded);
    }
    return main_lp;
}

void benders_decomposition(std::string data_dir) {
    OsiCpxSolverInterface main_solver;
    lp::LpDecatWithStock main_lp;
    int nb_cmd_base = 26460;
    Probleme reference(nb_cmd_base, 0.15, Livraison(1, 13, 1));
    read_and_gen_data_from_csv(reference, data_dir);
    main_lp.create_stock_variables(1);
    for (bool volu : {false, true}) {
        CoinPackedVector constraint;
        for (std::string lieu : LIEUX) {
            constraint.insert(main_lp.get_stock_var(lieu, volu), 1);
        }
        main_lp.add_constraint(constraint, 0.5, 1.5);
    }
    main_lp.solve();
    double borne_inf(main_lp.getc_objective_value()), borne_sup(INFINITY);

    std::vector<double> ratio_scenarios({0.9, 1, 1.1});
    std::vector<std::pair<Probleme, lp::LinearProblem>> scenarios;
    for (double ratio : ratio_scenarios) {
        lp::LinearProblem lp;
        Probleme pb(nb_cmd_base * ratio, 0.15, Livraison(1, 13, 1));
        read_and_gen_data_from_csv(pb, data_dir);
        lp.create_variables(pb);
        lp.create_constraints(pb);
        lp.solve();
        scenarios.push_back(std::make_pair(pb, lp));
    }
    while (borne_sup - borne_inf > 1e-3) {
        // set the new bounds of scenarios lp
        for (int i = 0; i < scenarios.size(); ++i) {
            Probleme &pb = scenarios[i].first;
            lp::LinearProblem &lp = scenarios[i].second;
            for (std::string lieu : LIEUX) {
                for (bool volu : {false, true}) {
                    int stock_constraint_idx =
                        lp.get_stock_constraint_idx(lieu, volu);
                    if (stock_constraint_idx != -1) {
                        int stock_var_idx = main_lp.get_stock_var(lieu, volu);
                        double upper = pb.getc_nb_articles(volu)
                                       * main_lp.get_var_value(stock_var_idx);
                        scenarios[i].second.set_row_upper(stock_constraint_idx,
                                                          upper);
                    }
                }
            }
        }
        // solve them
        for (int i = 0; i < scenarios.size(); ++i) {
            lp::LinearProblem &lp = scenarios[i].second;
            lp.resolve();
        }
        // add optimality cuts
        for (int i = 0; i < scenarios.size(); ++i) {
            lp::LinearProblem &lp = scenarios[i].second;
            Probleme &pb = scenarios[i].first;
            int cutVarIdx;
            double constant_term(lp.getc_objective_value());
            main_lp.add_var(cutVarIdx, 1);
            CoinPackedVector optimalityCut;
            optimalityCut.insert(cutVarIdx, -1);
            // loop on PFS & MAG std and volu; CAR volu only
            for (std::string lieu : LIEUX) {
                for (bool volu : {false, true}) {
                    if (lieu != "CAR" || volu) {
                        optimalityCut.insert(
                            main_lp.get_stock_var(lieu, volu),
                            -reference.getc_nb_articles(volu)
                                * lp.get_row_value(
                                    lp.get_stock_constraint_idx(lieu, volu)));
                        constant_term -=
                            -reference.getc_nb_articles(volu)
                            * lp.get_row_value(
                                lp.get_stock_constraint_idx(lieu, volu))
                            * main_lp.get_var_value(
                                main_lp.get_stock_var(lieu, volu));
                    }
                }
            }
            main_lp.add_cut(optimalityCut, -constant_term, main_lp.infinity());
            // main_lp.add_constraint(optimalityCut, -constant_term);
        }
        // solve master
        main_lp.resolve();
        // update upper lower bounds
        borne_inf = main_lp.getc_objective_value();
        for (int i = 0; i < scenarios.size(); ++i) {
            borne_sup =
                std::min(borne_sup, scenarios[i].second.getc_objective_value());
        }
        std::cout << borne_inf << " - " << borne_sup << std::endl;
    }
}