#include "lp.h"
#include "utils.h"
#include <sstream>

namespace lp {

LinearInterface::LinearInterface(OsiSolverInterface &solver) :
    solver_interface(solver),
    matrix(false, 0, 0) {}

LinearProblem::LinearProblem(OsiSolverInterface &solver) :
    LinearInterface(solver) {}

LpDecatWithStock::LpDecatWithStock(OsiSolverInterface &solver) :
    LinearProblem(solver) {}

void LinearInterface::load_problem() {
    solver_interface.loadProblem(matrix, col_lb.getElements(),
                                 col_ub.getElements(), objective.getElements(),
                                 row_lb.getElements(), row_ub.getElements());
}

void LinearInterface::add_var(int &idx,
                              double coef_obj,
                              double lower,
                              double upper) {

    col_lb.insert(col_lb.getNumElements(), lower);
    col_ub.insert(col_lb.getNumElements(),
                  upper == INFINITY ? infinity() : upper);
    objective.insert(col_lb.getNumElements(), coef_obj);
    matrix.appendCol(CoinPackedVector());
    idx = col_lb.getNumElements() - 1;
}

int LinearInterface::add_constraint(double lower, double upper) {
    row_lb.insert(matrix.getNumRows(),
                  lower == -INFINITY ? -infinity() : lower);
    row_ub.insert(matrix.getNumRows(), upper == INFINITY ? infinity() : upper);
    matrix.appendRow(CoinPackedVector());
    return matrix.getNumRows() - 1;
}

int LinearInterface::add_constraint(CoinPackedVector &coefs,
                                    double lower,
                                    double upper) {
    row_lb.insert(matrix.getNumRows(),
                  lower == -INFINITY ? -infinity() : lower);
    row_ub.insert(matrix.getNumRows(), upper == INFINITY ? infinity() : upper);
    matrix.appendRow(coefs);
    return matrix.getNumRows() - 1;
}

void LinearInterface::set_coef(int row_idx, int col_idx, double value) {
    matrix.modifyCoefficient(row_idx, col_idx, value);
}

double LinearInterface::infinity() {
    return solver_interface.getInfinity();
}
double LinearInterface::get_var_value(int col_idx) const {
    if (col_idx >= 0 && col_idx < solver_interface.getNumCols()) {
        return solver_interface.getColSolution()[col_idx];
    } else {
        std::cerr << "Tried to access variable out of range in LinearProblem "
                     "at index :"
                  << col_idx
                  << ", max index is : " << solver_interface.getNumCols()
                  << std::endl;
        return 0;
    }
}

void LinearProblem::load_data_in_lp(Probleme const &pb) {
    create_variables(pb);
    create_constraints(pb);
    load_problem();
}
void LpDecatWithStock::load_data_in_lp(Probleme const &pb) {
    create_stock_variables(0);
    create_variables(pb);
    create_constraints(pb);
    load_problem();
}

void LinearProblem::create_variables(Probleme const &pb) {
    Commande_Variable_map &demande_variables_map = get_var_map();
    for (CommandeType cmd : Probleme::commandes_set) {

        demande_variables_map[cmd] = {std::vector<Variable>(),
                                      std::vector<Variable>()};
        double quantite_std(pb.getc_quantite(cmd, false));
        double quantite_volu(pb.getc_quantite(cmd, true));
        for (int i = 0; i <= cmd.get_nb_articles(); i++) {
            for (Itineraire r : pb.getc_vec_itineraires()) {
                if (!r.is_volumineux()) {
                    if (cmd.get_delai() == r.get_delai()
                        && r.is_possible(i, cmd.get_nb_articles())
                        && cmd.get_heure() < r.get_cutoff()) {
                        int idx;
                        this->add_var(idx,
                                      quantite_std
                                          * get_prix_total_itineraire(
                                              pb, r, i, cmd.get_nb_articles()),
                                      0, 1);
                        demande_variables_map[cmd][0].push_back(
                            Variable(r, i, idx));
                    }
                } else {
                    for (std::string lieu : LIEUX) {
                        if (r.get_depart_volu() == lieu
                            && cmd.get_delai() == r.get_delai()
                            && r.is_possible(i, cmd.get_nb_articles(),
                                             r.get_depart_volu())
                            && cmd.get_heure() < r.get_cutoff()) {
                            int idx;
                            this->add_var(
                                idx,
                                quantite_volu
                                    * get_prix_total_itineraire(
                                        pb, r, i, cmd.get_nb_articles()),
                                0, 1);
                            demande_variables_map[cmd][1].push_back(
                                Variable(r, i, idx));
                        }
                    }
                }
            }
        }
    }
}

void LpDecatWithStock::create_stock_variables(double coef_obj) {
    int idx;
    for (std::string lieu : LIEUX) {
        add_var(idx, coef_obj, 0, 1);
        set_stock_var(lieu, false, idx);
        add_var(idx, coef_obj, 0, 1);
        set_stock_var(lieu, true, idx);
    }
}
void LinearProblem::create_constraints(Probleme const &pb) {
    fullfilment_constraint(pb);
    stock_constraint(pb);
}
namespace {

void quantities_from_pfs_and_mag(Probleme const &pb,
                                 LinearProblem &lin_pb,
                                 CoinPackedVector &vec_pfs,
                                 CoinPackedVector &vec_mag) {
    for (CommandeType cmd : Probleme::commandes_set) {
        double quantite_std = pb.getc_quantite(cmd, false);
        double quantite_volu = pb.getc_quantite(cmd, true);
        for (Variable var : lin_pb.get_var_list(cmd, false)) {
            vec_pfs.insert(var.problem_idx, quantite_std * var.i);
            vec_mag.insert(var.problem_idx,
                           quantite_std * (cmd.get_nb_articles() - var.i));
        }
        for (Variable var : lin_pb.get_var_list(cmd, true)) {
            vec_pfs.insert(var.problem_idx, quantite_volu * var.i);
            vec_mag.insert(var.problem_idx,
                           quantite_volu * (cmd.get_nb_articles() - var.i));
        }
    }
}

void quantities_volu(Probleme const &pb,
                     LinearProblem &lin_pb,
                     std::string lieu,
                     CoinPackedVector &vec_volu) {
    for (CommandeType cmd : Probleme::commandes_set) {
        double quantite_volu = pb.getc_quantite(cmd, true);
        for (Variable var : lin_pb.get_var_list(cmd, true)) {
            if (var.route.get_depart_volu() == lieu) {
                vec_volu.insert(var.problem_idx, quantite_volu);
            }
        }
    }
}
} // namespace

void LpDecatWithStock::stock_constraint(Probleme const &pb) {
    int nb_articles_std(pb.getc_nb_articles(false));
    int nb_articles_volu(pb.getc_nb_articles(true));
    CoinPackedVector vec_stock_pfs, vec_stock_mag;
    quantities_from_pfs_and_mag(pb, *this, vec_stock_pfs, vec_stock_mag);
    CoinPackedVector vec_stocks_var_std, vec_stocks_var_volu;
    for (std::string lieu : LIEUX) {
        vec_stocks_var_std.insert(get_stock_var(lieu, false), 1);
        vec_stocks_var_volu.insert(get_stock_var(lieu, true), 1);
    }

    vec_stock_pfs.insert(this->get_stock_var("PFS", false), -nb_articles_std);
    vec_stock_mag.insert(this->get_stock_var("Mag", false), -nb_articles_std);
    this->add_constraint(vec_stock_pfs, -infinity(), 0);
    this->add_constraint(vec_stock_mag, -infinity(), 0);

    CoinPackedVector vec_constraint_stock_volu;
    for (std::string lieu : LIEUX) {
        vec_constraint_stock_volu.clear();
        quantities_volu(pb, *this, lieu, vec_constraint_stock_volu);
        vec_constraint_stock_volu.insert(get_stock_var(lieu, true),
                                         -nb_articles_volu);
        this->add_constraint(vec_constraint_stock_volu, -infinity(), 0);
    }
}

void LinearProblem::stock_constraint(Probleme const &pb) {
    int nb_articles_std(pb.getc_nb_articles(false));
    int nb_articles_volu(pb.getc_nb_articles(true));
    CoinPackedVector vec_stock_pfs, vec_stock_mag;
    quantities_from_pfs_and_mag(pb, *this, vec_stock_pfs, vec_stock_mag);
    this->add_constraint(vec_stock_pfs, 0,
                         nb_articles_std * pb.getc_stocks().at("PFS")[0]);
    this->add_constraint(vec_stock_mag, 0,
                         nb_articles_std * pb.getc_stocks().at("Mag")[0]);

    CoinPackedVector vec_constraint_stock_volu;
    for (std::string lieu : LIEUX) {
        vec_constraint_stock_volu.clear();
        quantities_volu(pb, *this, lieu, vec_constraint_stock_volu);
        double stock_volu_max = pb.getc_stocks().at(lieu)[1] * nb_articles_volu;
        this->add_constraint(vec_constraint_stock_volu, 0, stock_volu_max);
    }
}

void LinearProblem::fullfilment_constraint(Probleme const &pb) {
    for (CommandeType cmd : Probleme::commandes_set) {
        CoinPackedVector vec_constraint_fullfillment_std;
        CoinPackedVector vec_constraint_fullfillment_volu;
        for (Variable var : get_var_list(cmd, false)) {
            vec_constraint_fullfillment_std.insert(var.problem_idx, 1);
        }
        for (Variable var : get_var_list(cmd, true)) {
            vec_constraint_fullfillment_volu.insert(var.problem_idx, 1);
        }
        this->add_constraint(vec_constraint_fullfillment_std, 1, 1);
        this->add_constraint(vec_constraint_fullfillment_volu, 1, 1);
    }
}

std::map<std::string, double>
    get_map_solution(Probleme const &pb, LinearProblem &lin_pb, bool volu) {
    std::map<std::string, double> map_results;
    std::stringstream buffer;
    for (CommandeType cmd : Probleme::commandes_set) {
        for (lp::Variable v : lin_pb.get_var_map().at(cmd)[volu]) {
            if (lin_pb.get_var_value(v.problem_idx) > 0.001) {
                buffer.clear();
                buffer.str(std::string());
                buffer << v.route << ", count : ";
                if (map_results.contains(buffer.str())) {
                    map_results[buffer.str()] +=
                        lin_pb.get_var_value(v.problem_idx)
                        * pb.getc_quantite(cmd, volu);
                } else {
                    map_results[buffer.str()] =
                        lin_pb.get_var_value(v.problem_idx)
                        * pb.getc_quantite(cmd, volu);
                }
            }
        }
    }
    return map_results;
}

std::map<std::string, double> get_map_prep_costs(Probleme const &pb,
                                                 LinearProblem &lin_pb) {
    std::map<std::string, double> preparation_costs(
        {{"Mag", 0}, {"PFS", 0}, {"CAR", 0}});
    for (CommandeType cmd : Probleme::commandes_set) {
        for (lp::Variable v : lin_pb.get_var_map().at(cmd)[0]) {
            for (std::string lieu : LIEUX) {
                preparation_costs[lieu] +=
                    pb.getc_quantite(cmd, false)
                    * lin_pb.get_var_value(v.problem_idx)
                    * get_prix_prepa_itineraire(pb, v.route, v.i,
                                                cmd.get_nb_articles(), lieu);
            }
        }
        for (lp::Variable v : lin_pb.get_var_map().at(cmd)[1]) {
            for (std::string lieu : LIEUX) {
                preparation_costs[lieu] +=
                    pb.getc_quantite(cmd, true)
                    * lin_pb.get_var_value(v.problem_idx)
                    * get_prix_prepa_itineraire(pb, v.route, v.i,
                                                cmd.get_nb_articles(), lieu);
            }
        }
    }
    return preparation_costs;
}

std::string get_str_solution(Probleme const &pb, LinearProblem &lin_pb) {
    std::map<std::string, double> map_results_std(
        get_map_solution(pb, lin_pb, false));
    std::map<std::string, double> map_results_volu(
        get_map_solution(pb, lin_pb, true));
    std::map<std::string, double> preparation_costs(
        get_map_prep_costs(pb, lin_pb));

    std::stringstream buffer;
    buffer << "nb cmd : " << pb.get_nb_cmd()
           << ", ratio volu : " << pb.get_ratio_volu() << std::endl;
    buffer << std::endl;

    buffer << "stocks dispo (std,volu) : " << pb.getc_stocks() << std::endl;
    buffer << std::endl;

    buffer << "cout de préparation (std,volu) : " << pb.getc_prix_preration()
           << std::endl;
    buffer << "couts totaux de préparation : " << preparation_costs
           << std::endl;
    buffer << std::endl;

    for (auto iter : map_results_std) {
        buffer << iter.first << iter.second << std::endl;
    }
    buffer << std::endl;

    for (auto iter : map_results_volu) {
        buffer << iter.first << iter.second << std::endl;
    }
    buffer << std::endl;

    buffer << "valeur fonction objectif : "
           << lin_pb.get_solver_interface().getObjValue() << std::endl;
    return buffer.str();
}

std::string get_str_solution(Probleme const &pb, LpDecatWithStock &lin_pb) {
    std::map<std::string, double> map_results_std(
        get_map_solution(pb, lin_pb, false));
    std::map<std::string, double> map_results_volu(
        get_map_solution(pb, lin_pb, true));
    std::map<std::string, double> preparation_costs(
        get_map_prep_costs(pb, lin_pb));
    std::map<std::string, std::array<double, 2>> stock_var_values;

    for (auto iter : lin_pb.get_stock_var_map()) {
        stock_var_values[iter.first][false] =
            lin_pb.get_var_value(iter.second[false]);
        stock_var_values[iter.first][true] =
            lin_pb.get_var_value(iter.second[true]);
    }

    std::stringstream buffer;
    buffer << "nb cmd : " << pb.get_nb_cmd()
           << ", ratio volu : " << pb.get_ratio_volu() << std::endl;
    buffer << std::endl;

    buffer << "stocks variables (std,volu) : " << stock_var_values << std::endl;
    buffer << std::endl;

    buffer << "cout de préparation (std,volu) : " << pb.getc_prix_preration()
           << std::endl;

    buffer << "couts totaux de préparation : " << preparation_costs
           << std::endl;

    buffer << std::endl;

    for (auto iter : map_results_std) {
        buffer << iter.first << iter.second << std::endl;
    }
    buffer << std::endl;

    for (auto iter : map_results_volu) {
        buffer << iter.first << iter.second << std::endl;
    }
    buffer << std::endl;

    buffer << "valeur fonction objectif : "
           << lin_pb.get_solver_interface().getObjValue() << std::endl;
    return buffer.str();
}
} // namespace lp