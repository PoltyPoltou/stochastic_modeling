#include "stochastic.h"
#include "data.h"
#include <cmath>
#include <iostream>
inline double normal_probabilty(double x, double mean, double var) {
    return (std::erf((x - mean) / var) + 1) / 2;
}

lp::LpDecatScenarios stochastic_problem(std::string data_dir,
                                        OsiSolverInterface &solver_interface,
                                        ProblemeStochastique &pb_loaded) {
    lp::LpDecatScenarios main_lp(solver_interface);
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
