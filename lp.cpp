#include "lp.h"
namespace lp {

LinearProblem::LinearProblem(OsiSolverInterface &solver) :
    solver_interface(solver) {}

int LinearProblem::add_var(double coef_obj, double lower, double upper) {
    col_lb.insert(matrix.getNumCols(), lower);
    col_ub.insert(matrix.getNumCols(), upper == INFINITY ? infinity() : upper);
    objective.insert(matrix.getNumCols(), coef_obj);
    matrix.appendCol(CoinPackedVector());
    return matrix.getNumCols() - 1;
}

int LinearProblem::add_constraint(double lower, double upper) {
    row_lb.insert(matrix.getNumRows(),
                  lower == -INFINITY ? -infinity() : lower);
    row_ub.insert(matrix.getNumRows(), upper == INFINITY ? infinity() : upper);
    matrix.appendRow(CoinPackedVector());
    return matrix.getNumRows() - 1;
}

int LinearProblem::add_constraint(CoinPackedVector &coefs,
                                  double lower,
                                  double upper) {
    row_lb.insert(matrix.getNumRows(),
                  lower == -INFINITY ? -infinity() : lower);
    row_ub.insert(matrix.getNumRows(), upper == INFINITY ? infinity() : upper);
    matrix.appendRow(coefs);
    return matrix.getNumRows() - 1;
}

void LinearProblem::set_coef(int row_idx, int col_idx, double value) {
    matrix.modifyCoefficient(row_idx, col_idx, value);
}

double LinearProblem::infinity() {
    return solver_interface.getInfinity();
}


} // namespace lp