#include "lp.h"
namespace lp {

LinearProblem::LinearProblem(OsiSolverInterface &solver) :
    solver_interface(solver) {}

void LinearProblem::load_problem() {
    solver_interface.loadProblem(matrix, col_lb.getElements(),
                                 col_ub.getElements(), objective.getElements(),
                                 row_lb.getElements(), row_ub.getElements());
}

void LinearProblem::add_var(int &idx,
                            double coef_obj,
                            double lower,
                            double upper) {
    col_lb.insert(matrix.getNumCols(), lower);
    col_ub.insert(matrix.getNumCols(), upper == INFINITY ? infinity() : upper);
    objective.insert(matrix.getNumCols(), coef_obj);
    matrix.appendCol(CoinPackedVector());
    idx = matrix.getNumCols() - 1;
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

void load_data_in_lp(Probleme const &pb, LinearProblem &lin_pb) {
    Commande_Variable_map cmd_var_map(create_variables(pb, lin_pb));
    create_constraints(pb, lin_pb, cmd_var_map);
}
Commande_Variable_map create_variables(Probleme const &pb,
                                       LinearProblem &lin_pb) {
    Commande_Variable_map demande_variables_map;
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
                        lin_pb.add_var(idx,
                                       quantite_std
                                           * get_prix_total_itineraire(
                                               pb, r, i, cmd.get_nb_articles()),
                                       0, 1);
                        demande_variables_map[cmd][0].push_back(
                            Variable(r, i, idx));
                    }
                } else {
                    for (std::string lieu : LIEUX_VOLU) {
                        if (r.get_depart_volu() == lieu
                            && cmd.get_delai() == r.get_delai()
                            && r.is_possible(i, cmd.get_nb_articles(),
                                             r.get_depart_volu())
                            && cmd.get_heure() < r.get_cutoff()) {
                            int idx;
                            lin_pb.add_var(
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
    return demande_variables_map;
}

void create_constraints(Probleme const &pb,
                        LinearProblem &lin_pb,
                        Commande_Variable_map &cmd_var_map) {
    stock_constraint(pb, lin_pb, cmd_var_map);
    fullfilment_constraint(pb, lin_pb, cmd_var_map);
}

void stock_constraint(Probleme const &pb,
                      LinearProblem &lin_pb,
                      Commande_Variable_map &cmd_var_map) {
    int nb_articles_std(pb.getc_nb_articles(false));
    int nb_articles_volu(pb.getc_nb_articles(true));
    int constraint_sotck_pfs(lin_pb.add_constraint(
        0, nb_articles_std * pb.getc_stocks().at("PFS")[0]));
    int constraint_sotck_mag(lin_pb.add_constraint(
        0, nb_articles_std * pb.getc_stocks().at("Mag")[0]));

    for (std::string lieu : LIEUX_VOLU) {
        double stock_volu_max = pb.getc_stocks().at(lieu)[0] * nb_articles_volu;
        int constraint_stock_volu(lin_pb.add_constraint(0, stock_volu_max));
        for (CommandeType cmd : Probleme::commandes_set) {
            for (Variable var : cmd_var_map.at(cmd)[1]) {
                lin_pb.set_coef(constraint_stock_volu, var.problem_idx, 1);
                lin_pb.set_coef(constraint_sotck_pfs, var.problem_idx, var.i);
                lin_pb.set_coef(constraint_sotck_mag, var.problem_idx,
                                cmd.get_nb_articles() - var.i);
            }
            for (Variable var : cmd_var_map.at(cmd)[0]) {
                lin_pb.set_coef(constraint_sotck_pfs, var.problem_idx, var.i);
                lin_pb.set_coef(constraint_sotck_mag, var.problem_idx,
                                cmd.get_nb_articles() - var.i);
            }
        }
    }
}

void fullfilment_constraint(Probleme const &pb,
                            LinearProblem &lin_pb,
                            Commande_Variable_map &cmd_var_map) {
    for (CommandeType cmd : Probleme::commandes_set) {
        int constraint_fullfillment_std(lin_pb.add_constraint(1, 1));
        int constraint_fullfillment_volu(lin_pb.add_constraint(1, 1));
        for (Variable var : cmd_var_map.at(cmd)[0]) {
            lin_pb.set_coef(constraint_fullfillment_std, var.problem_idx, 1);
        }
        for (Variable var : cmd_var_map.at(cmd)[1]) {
            lin_pb.set_coef(constraint_fullfillment_volu, var.problem_idx, 1);
        }
    }
}

} // namespace lp