#include "benders.h"
#include <math.h>
void benders_decomposition(lp::LinearInterface main_lp,
                           std::vector<int> scenarios,
                           std::vector<double> proba_scenarios,
                           Probleme baseProblem) {
    main_lp.get_solver_interface().initialSolve();
    double epsilon = 1e-6;
    double borne_inf(main_lp.get_solver_interface().getObjValue()),
        borne_sup(INFINITY);
    do {
        main_lp.get_solver_interface().resolve();
        borne_inf = main_lp.get_solver_interface().getObjValue();

    } while (borne_sup - borne_inf > epsilon);
}