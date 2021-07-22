#pragma once
#include "coin/CoinPackedMatrix.hpp"
#include "coin/CoinPackedVector.hpp"
#include "coin/OsiCpxSolverInterface.hpp"
#include "coin_util.h"
#include "probleme.h"
#include <array>
#include <cmath>
#include <unordered_map>
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
    Commande_Variable_map var_map;
    std::map<std::string, std::array<int, 2>> stock_var_map;

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
    Commande_Variable_map &get_var_map() { return var_map; };
    std::map<std::string, std::array<int, 2>> get_stock_var_map() const {
        return stock_var_map;
    };
    void set_stock_var(std::string lieu, bool volu, int idx) {
        stock_var_map[lieu][volu] = idx;
    }
    int get_stock_var(std::string lieu, bool volu) const {
        return stock_var_map.at(lieu).at(volu);
    }
    void add_var(int &idx,
                 double coef_obj,
                 double lower = 0,
                 double upper = INFINITY);
    int add_constraint(double lower = -INFINITY, double upper = INFINITY);
    int add_constraint(CoinPackedVector &coefs,
                       double lower = -INFINITY,
                       double upper = INFINITY);
    double get_var_value(int col_idx) const;
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

void load_data_in_lp(Probleme const &pb,
                     LinearProblem &lin_pb,
                     bool stock_variables = false);

void create_variables(Probleme const &pb,
                      LinearProblem &lin_pb,
                      bool stock_variables);

void create_constraints(Probleme const &pb,
                        LinearProblem &lin_pb,
                        bool stock_variables);

void stock_var_constraint(LinearProblem &lin_pb);

void stock_constraint(Probleme const &pb,
                      LinearProblem &lin_pb,
                      bool stock_variables);

void fullfilment_constraint(Probleme const &pb, LinearProblem &lin_pb);

std::map<std::string, double>
    get_map_solution(Probleme const &pb, LinearProblem &lin_pb, bool volu);
std::map<std::string, double> get_map_prep_costs(Probleme const &pb,
                                                 LinearProblem &lin_pb);

std::string get_str_solution(Probleme const &pb, LinearProblem &lin_pb);

} // namespace lp
