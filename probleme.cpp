#include "probleme.h"

std::set<CommandeType, Compare_CmdType> Probleme::commandes_set =
    std::set<CommandeType, Compare_CmdType>();

Probleme::Probleme(int n_cmd, double ratio_volumineux, Livraison car) :
    nb_cmd(n_cmd),
    ratio_volu(ratio_volumineux),
    livraison_car(car) {
    if (Probleme::commandes_set.empty()) {
        for (int n = 1; n < Probleme::articles_max; n++) {
            for (int h = 0; h < 24; h++) {
                for (int d = Probleme::delai_min; d <= Probleme::delai_max;
                     d++) {
                    Probleme::commandes_set.insert(CommandeType(n, h, d));
                }
            }
        }
    }
}

