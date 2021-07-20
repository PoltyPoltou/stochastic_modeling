#pragma once
#include "coin/CoinPackedMatrix.hpp"
#include "coin/CoinPackedVector.hpp"
#include "coin/OsiCpxSolverInterface.hpp"
#include "coin_util.h"
#include "probleme.h"
#include <array>
#include <cmath>
#include <vector>

namespace lp {

class LinearProblem;
class Variable;

typedef std::map<CommandeType,
                 std::array<std::vector<Variable>, 2>,
                 Compare_CmdType>
    Commande_Variable_map; // links CommandeType to 2 vectors of lp variables,
                           // 1st array is for std; 2nd is for volu

class LinearProblem {
  private:
    OsiSolverInterface &solver_interface;
    CoinPackedMatrix matrix;
    CoinPackedVector objective, col_lb, col_ub, row_lb, row_ub;
    std::vector<std::string> var_names, constraints_names;

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
    void add_var(int &idx,
                 double coef_obj,
                 double lower = 0,
                 double upper = INFINITY);
    int add_constraint(double lower = -INFINITY, double upper = INFINITY);
    int add_constraint(CoinPackedVector &coefs,
                       double lower = -INFINITY,
                       double upper = INFINITY);
    void set_coef(int row_idx, int col_idx, double value);

    double infinity();
};

struct Variable {
    Itineraire route;
    int i;
    int problem_idx;
    Variable();
    Variable(Itineraire &r, int i, int idx) :
        route(r),
        i(i),
        problem_idx(idx) {};
};

Commande_Variable_map load_data_in_lp(Probleme const &pb,
                                      LinearProblem &lin_pb);

Commande_Variable_map create_variables(Probleme const &pb,
                                       LinearProblem &lin_pb);

void create_constraints(Probleme const &pb,
                        LinearProblem &lin_pb,
                        Commande_Variable_map &cmd_var_map);

void stock_constraint(Probleme const &pb,
                      LinearProblem &lin_pb,
                      Commande_Variable_map &cmd_var_map);

void fullfilment_constraint(Probleme const &pb,
                            LinearProblem &lin_pb,
                            Commande_Variable_map &cmd_var_map);

} // namespace lp
