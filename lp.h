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

class Variable;

typedef std::map<CommandeType,
                 std::array<std::vector<Variable>, 2>,
                 Compare_CmdType>
    Commande_Variable_map; // links CommandeType to 2 vectors of lp variables,
                           // 1st array is for std; 2nd is for volu

class LinearInterface {
  private:
    OsiCpxSolverInterface solver_interface;
    CoinPackedMatrix matrix;
    CoinPackedVector objective, col_lb, col_ub, row_lb, row_ub;

  public:
    LinearInterface();
    OsiCpxSolverInterface &get_solver_interface() { return solver_interface; };
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
    int add_constraint(CoinPackedVector &coefs,
                       double lower = -INFINITY,
                       double upper = INFINITY);
    double get_var_value(int col_idx) const;
    double get_row_value(int row_idx) const;
    void solve() { solver_interface.initialSolve(); };
    void resolve() { solver_interface.resolve(); };
    double getc_objective_value() const {
        return solver_interface.getObjValue();
    };
    int getc_nb_rows() const { return solver_interface.getNumRows(); };
    void set_row_bounds(int idx, double lower, double upper);
    void set_row_upper(int idx, double upper);
    void add_cut(CoinPackedVector &coefs, double lower, double upper);
    double infinity();
};

class LinearProblem : public LinearInterface {
  private:
    Commande_Variable_map var_map;
    std::unordered_map<std::string, std::array<int, 2>> stock_constraint_idx;
    std::unordered_map<std::string, std::array<int, 2>> opt_recours_var_idx;

  protected:
    void create_variables(Commande_Variable_map &demande_variables_map,
                          Probleme const &pb,
                          double factor = 1);

  public:
    LinearProblem();
    Commande_Variable_map &get_var_map() { return var_map; };
    std::vector<Variable> const &get_var_list(CommandeType cmd, bool volu) {
        return var_map.at(cmd).at(volu);
    };
    int get_stock_constraint_idx(std::string const &lieu, bool volu) const {
        return stock_constraint_idx.at(lieu)[volu];
    }
    virtual void load_data_in_lp(Probleme const &pb);
    virtual void create_variables(Probleme const &pb);
    virtual void stock_constraint(Probleme const &pb);
    void create_opt_recours_var(double coef_obj);
    void create_constraints(Probleme const &pb);
    void fullfilment_constraint(Probleme const &pb);
};

class LpDecatWithStock : public LinearProblem {
  private:
    std::map<std::string, std::array<int, 2>> stock_var_map;

  public:
    LpDecatWithStock();
    virtual void load_data_in_lp(Probleme const &pb);
    virtual void stock_constraint(Probleme const &pb);
    virtual void stock_constraint(ProblemeStochastique const &pb);
    void create_stock_variables(double coef);

    std::map<std::string, std::array<int, 2>> get_stock_var_map() const {
        return stock_var_map;
    };
    void set_stock_var(std::string lieu, bool volu, int idx) {
        stock_var_map[lieu][volu] = idx;
    }
    int get_stock_var(std::string lieu, bool volu) const {
        return stock_var_map.at(lieu).at(volu);
    }
};

class LpDecatScenarios : public LpDecatWithStock {
  private:
    std::map<int, Commande_Variable_map> scenario_var_map;
    std::map<int, double> scenario_proba_map;

  public:
    LpDecatScenarios();
    virtual void create_variables(ProblemeStochastique const &pb, double proba);
    void set_scenario(int scenario);
    int get_nb_scenarios() const { return scenario_var_map.size(); };
    std::vector<int> get_scenarios() const;
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

std::map<std::string, double>
    get_map_solution(Probleme const &pb, LinearProblem &lin_pb, bool volu);
std::map<std::string, double> get_map_prep_costs(Probleme const &pb,
                                                 LinearProblem &lin_pb);
std::map<std::string, double> get_map_solution(ProblemeStochastique const &pb,
                                               LpDecatScenarios &lin_pb,
                                               bool volu);
std::map<std::string, double> get_map_prep_costs(ProblemeStochastique const &pb,
                                                 LpDecatScenarios &lin_pb);

std::string get_str_solution(Probleme const &pb, LinearProblem &lin_pb);
std::string get_str_solution(Probleme const &pb, LpDecatWithStock &lin_pb);
std::string get_str_solution(ProblemeStochastique &pb,
                             LpDecatScenarios &lin_pb);
std::string get_str_solution(ProblemeStochastique &pb,
                             LpDecatScenarios &lin_pb,
                             int scenario);

} // namespace lp
