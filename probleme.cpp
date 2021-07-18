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

// NOT TESTED
double get_prix_prepa_itineraire(
    Probleme &pb, Itineraire &itin, int i, int n, std::string lieu) {
    int quantite = 0;
    int quantite_volu =
        itin.is_volumineux() && (itin.get_depart_volu() == lieu);
    if (lieu == "PFS") {
        quantite = i;
    } else if (lieu == "Mag") {
        quantite = n - i;
    } else { // lieu =="CAR"
        quantite = 0;
    }
    return quantite * pb.get_prix_preration()[lieu][0]
           + quantite_volu * pb.get_prix_preration()[lieu][1];
}
// NOT TESTED
double get_prix_total_itineraire(Probleme &pb, Itineraire &itin, int i, int n) {
    return itin.get_prix_livraison()
           + get_prix_prepa_itineraire(pb, itin, i, n, "PFS")
           + get_prix_prepa_itineraire(pb, itin, i, n, "Mag")
           + get_prix_prepa_itineraire(pb, itin, i, n, "CAR");
}