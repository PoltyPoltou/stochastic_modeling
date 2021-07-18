#pragma once
#include "coin/CoinPackedMatrix.hpp"
#include "coin/CoinPackedVector.hpp"
#include "coin/OsiCpxSolverInterface.hpp"
#include "coin_util.h"
#include "probleme.h"
#include <cmath>
namespace lp {

class LinearProblem {
  private:
    OsiSolverInterface &solver_interface;
    CoinPackedMatrix matrix;
    CoinPackedVector objective, col_lb, col_ub, row_lb, row_ub;

  public:
    LinearProblem(OsiSolverInterface &solver);
    void load_problem();
    OsiSolverInterface &get_solver_interface() { return solver_interface; };
    CoinPackedMatrix &get_matrix() { return matrix; };
    CoinPackedVector &get_objective() { return objective; };
    CoinPackedVector &get_col_lb() { return col_lb; };
    CoinPackedVector &get_col_ub() { return col_ub; };
    CoinPackedVector &get_row_lb() { return row_lb; };
    CoinPackedVector &get_row_ub() { return row_ub; };
    int add_var(double coef_obj, double lower = 0, double upper = INFINITY);
    int add_constraint(double lower = -INFINITY, double upper = INFINITY);
    int add_constraint(CoinPackedVector &coefs,
                       double lower = -INFINITY,
                       double upper = INFINITY);
    void set_coef(int row_idx, int col_idx, double value);

    double infinity();
};

} // namespace lp
