#include "stochastic.h"
#include "data.h"

lp::LinearProblem stochastic_problem(std::string data_dir,
                                     OsiSolverInterface &solver_interface) {
    Probleme pb(26460, 0.15, Livraison(1, 13, 1));
    read_and_gen_data_from_csv(pb, data_dir);
    lp::LinearProblem main_lp(solver_interface);
    std::vector<int> scenarios({26460});

    lp::create_stock_variables(main_lp, scenarios.size());

    for (int n_cmd_scenario : scenarios) {
        pb.set_nb_cmd_stochastic(n_cmd_scenario);
        lp::create_variables(pb, main_lp);
        lp::fullfilment_constraint(pb, main_lp);
        lp::stock_constraint(pb, main_lp, true);
    }
    return main_lp;
}
