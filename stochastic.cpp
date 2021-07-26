#include "stochastic.h"
#include "data.h"

lp::LpDecatScenarios stochastic_problem(std::string data_dir,
                                        OsiSolverInterface &solver_interface,
                                        ProblemeStochastique &pb_loaded) {
    lp::LpDecatScenarios main_lp(solver_interface);
    std::vector<int> scenarios({int(26460 * 0.9), 26460, int(26460 * 1.1)});

    main_lp.create_stock_variables(1);

    for (int n_cmd_scenario : scenarios) {
        pb_loaded.set_nb_cmd_mesured(n_cmd_scenario);
        main_lp.create_variables(pb_loaded);
        main_lp.fullfilment_constraint(pb_loaded);
        main_lp.stock_constraint(pb_loaded);
    }
    return main_lp;
}
