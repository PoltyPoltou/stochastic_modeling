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
    create_variables(pb, lin_pb);
    create_constraints(pb, lin_pb);
}
void create_variables(Probleme const &pb, LinearProblem &lin_pb) {
    std::map<CommandeType, std::vector<Variable>, Compare_CmdType>
        demande_variables_map;
    for (CommandeType cmd : Probleme::commandes_set) {

        demande_variables_map[cmd] = std::vector<Variable>();
        double quantite_std = pb.get_nb_cmd() * (1 - pb.get_ratio_volu())
                              * pb.getc_demande().at(cmd)[0];
        double quantite_volu = pb.get_nb_cmd() * pb.get_ratio_volu()
                               * pb.getc_demande().at(cmd)[1];

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
                        demande_variables_map[cmd].push_back(
                            Variable(r, i, idx));
                    }
                } else {
                    for (std::string lieu : LIEUX_VOLU) {
                        if (r.get_depart_volu() == lieu
                            && cmd.get_delai() == r.get_delai()
                            && r.is_possible(i, cmd.get_nb_articles())
                            && cmd.get_heure() < r.get_cutoff()) {
                            int idx;
                            lin_pb.add_var(
                                idx,
                                quantite_volu
                                    * get_prix_total_itineraire(
                                        pb, r, i, cmd.get_nb_articles()),
                                0, 1);
                            demande_variables_map[cmd].push_back(
                                Variable(r, i, idx));
                        }
                    }
                }
            }
        }
    }
}
void create_constraints(Probleme const &pb, LinearProblem &lin_pb) {}

} // namespace lp