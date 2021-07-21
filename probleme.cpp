#include "probleme.h"

std::set<CommandeType, Compare_CmdType> Probleme::commandes_set =
    std::set<CommandeType, Compare_CmdType>();

Probleme::Probleme(int n_cmd, double ratio_volumineux, Livraison car) :
    nb_cmd(n_cmd),
    ratio_volu(ratio_volumineux),
    livraison_car(car) {
    if (Probleme::commandes_set.empty()) {
        for (int n = 1; n <= Probleme::articles_max; n++) {
            for (int h = 0; h < 24; h++) {
                for (int d = Probleme::delai_min; d <= Probleme::delai_max;
                     d++) {
                    Probleme::commandes_set.insert(CommandeType(n, h, d));
                }
            }
        }
    }
    prix_preration["PFS"] = {1.86, 1.98};
    prix_preration["Mag"] = {3.17, 3.37};
    prix_preration["CAR"] = {0, 1.98};
    stocks["PFS"] = {0.75, 0.2};
    stocks["Mag"] = {0.3, 1};
    stocks["CAR"] = {0, 1};
}

double Probleme::getc_quantite(CommandeType const &cmd, bool volu) const {
    double ratio = volu ? ratio_volu : 1 - ratio_volu;
    return nb_cmd * ratio * getc_demande().at(cmd)[volu];
}

int Probleme::getc_nb_articles(bool volu) const {
    if (volu) {
        return nb_cmd * ratio_volu;
    }
    double articles_cmd_std(0);
    double articles_cmd_volu(0);
    for (CommandeType cmd : Probleme::commandes_set) {
        // multiplications are factorised at most to increase numeric accuracy
        articles_cmd_std +=
            getc_demande().at(cmd)[false] * cmd.get_nb_articles();
        articles_cmd_volu +=
            getc_demande().at(cmd)[true] * cmd.get_nb_articles();
        }
    return nb_cmd
           * (ratio_volu * articles_cmd_volu
              + articles_cmd_std * (1 - ratio_volu));
    }

double get_prix_prepa_itineraire(
    Probleme const &pb, Itineraire &itin, int i, int n, std::string lieu) {
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
    return quantite * pb.getc_prix_preration().at(lieu)[0]
           + quantite_volu * pb.getc_prix_preration().at(lieu)[1];
}
// NOT TESTED
double get_prix_total_itineraire(Probleme const &pb,
                                 Itineraire &itin,
                                 int i,
                                 int n) {
    return itin.get_prix_livraison()
           + get_prix_prepa_itineraire(pb, itin, i, n, "PFS")
           + get_prix_prepa_itineraire(pb, itin, i, n, "Mag")
           + get_prix_prepa_itineraire(pb, itin, i, n, "CAR");
}