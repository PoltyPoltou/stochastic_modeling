#include "stochastic.h"
#include "data.h"

lp::LpDecatWithStock stochastic_problem(std::string data_dir,
                                        OsiSolverInterface &solver_interface) {
    Probleme pb(26460, 0.15, Livraison(1, 13, 1));
    read_and_gen_data_from_csv(pb, data_dir);
    lp::LpDecatWithStock main_lp(solver_interface);
    std::vector<int> scenarios({26460});

    main_lp.create_stock_variables(10);

    for (int n_cmd_scenario : scenarios) {
        pb.set_nb_cmd_stochastic(n_cmd_scenario);
        main_lp.create_variables(pb);
        main_lp.fullfilment_constraint(pb);
        main_lp.stock_constraint(pb);
    }
    return main_lp;
}
