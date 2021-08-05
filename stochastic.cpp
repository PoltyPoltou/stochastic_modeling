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
    std::vector<double> scenarios({1, 1.1});
    main_lp.create_stock_variables(
        pb_loaded.getc_nb_articles()
        * 0); // coût par 100% de distribution de stock
    main_lp.create_opt_recours_var(
        10); // coût par article pour étendre le stock (backlog)
    for (double ratio : scenarios) {
        pb_loaded.set_nb_cmd_mesured(ratio * pb_loaded.get_nb_cmd());
        main_lp.create_variables(pb_loaded, 1. / scenarios.size());
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
    main_lp.create_stock_variables(0);
    main_lp.solve();
    double borne_inf(main_lp.getc_objective_value()), borne_sup(INFINITY);

    std::vector<double> ratio_scenarios({1, 1.1});
    std::vector<std::pair<Probleme, lp::LinearProblem>> scenarios;
    std::vector<int> scenarios_optimality_var_idx;
    for (double ratio : ratio_scenarios) {
        lp::LinearProblem lp;
        Probleme pb(nb_cmd_base * ratio, 0.15, Livraison(1, 13, 1));
        read_and_gen_data_from_csv(pb, data_dir);
        lp.create_opt_recours_var(10);
        lp.create_variables(pb);
        lp.create_constraints(pb);
        lp.solve();
        scenarios.push_back(std::make_pair(pb, lp));
        int var_idx;
        main_lp.add_var(var_idx, 1. / ratio_scenarios.size());
        scenarios_optimality_var_idx.push_back(var_idx);
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
                        double upper = reference.getc_nb_articles(volu)
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
            if (!lp.get_solver_interface().isProvenOptimal()) {
                std::cout << "Failed to find optimality in scenario"
                          << std::endl;
            }
        }
        // add optimality cuts
        for (int i = 0; i < scenarios.size(); ++i) {
            lp::LinearProblem &lp = scenarios[i].second;
            Probleme &pb = scenarios[i].first;
            double constant_term(lp.getc_objective_value());
            CoinPackedVector optimalityCut;
            optimalityCut.insert(scenarios_optimality_var_idx[i], 1);
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
            main_lp.add_cut(optimalityCut, constant_term, main_lp.infinity());
            // main_lp.add_constraint(optimalityCut, -constant_term);
        }
        // solve master
        main_lp.resolve();
        // update upper lower bounds
        borne_inf = main_lp.getc_objective_value();
        borne_sup = 0;
        for (int i = 0; i < scenarios.size(); ++i) {
            borne_sup +=
                scenarios[i].second.getc_objective_value() / scenarios.size();
        }
        std::cout << borne_inf << " - " << borne_sup << std::endl;
    }
}