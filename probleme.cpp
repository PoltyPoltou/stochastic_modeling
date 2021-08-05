#include "probleme.h"
#include <math.h>
std::set<CommandeType, Compare_CmdType> Probleme::commandes_set =
    std::set<CommandeType, Compare_CmdType>();

Probleme::Probleme(int n_cmd, double ratio_volumineux, Livraison car) :
    Probleme(n_cmd,
             ratio_volumineux,
             car,
             {0.75, 0.2},
             {0.3, 0.5},
             {0, 0.5},
             {1.86, 1.98},
             {3.17, 3.37},
             {0, 1.98}) {}

Probleme::Probleme(int n_cmd,
                   double ratio_volumineux,
                   Livraison car,
                   std::array<double, 2> stock_pfs,
                   std::array<double, 2> stock_mag,
                   std::array<double, 2> stock_car,
                   std::array<double, 2> prix_prepa_pfs,
                   std::array<double, 2> prix_prepa_mag,
                   std::array<double, 2> prix_prepa_car) :
    nb_cmd(n_cmd),
    ratio_volu(ratio_volumineux),
    livraison_car(car) {
    // init of the static member commandes_set
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

    stocks["PFS"] = stock_pfs;
    stocks["Mag"] = stock_mag;
    stocks["CAR"] = stock_car;
    prix_preration["PFS"] = prix_prepa_pfs;
    prix_preration["Mag"] = prix_prepa_mag;
    prix_preration["CAR"] = prix_prepa_car;
}

double Probleme::getc_quantite(CommandeType const &cmd, bool volu) const {
    return quantite.at(cmd)[volu];
}

void Probleme::compute_quantite(CommandeType const &cmd) {
    quantite[cmd] = {nb_cmd * (1 - ratio_volu) * demande.at(cmd)[0],
                     nb_cmd * ratio_volu * demande.at(cmd)[1]};
}

int Probleme::getc_nb_articles(bool volu) const {
    if (volu) {
        return nb_cmd * ratio_volu;
    }
    double articles_cmd_std(0);
    double articles_cmd_volu(0);
    for (CommandeType cmd : Probleme::commandes_set) {
        // multiplications are factorised at most to increase numeric accuracy
        articles_cmd_std += getc_demande(cmd, false) * cmd.get_nb_articles();
        articles_cmd_volu += getc_demande(cmd, true) * cmd.get_nb_articles();
    }
    return nb_cmd
           * (ratio_volu * articles_cmd_volu
              + articles_cmd_std * (1 - ratio_volu));
}

void Probleme::set_demande(CommandeType const &cmd, double std, double volu) {
    demande[cmd] = {std, volu};
    compute_quantite(cmd);
}

int ProblemeStochastique::getc_nb_articles(bool volu) const {
    if (volu) {
        return get_nb_cmd() * get_ratio_volu();
    }
    double articles_cmd_std(0);
    double articles_cmd_volu(0);
    for (CommandeType cmd : Probleme::commandes_set) {
        // multiplications are factorised at most to increase numeric accuracy
        articles_cmd_std += getc_demande(cmd, false) * cmd.get_nb_articles();
        articles_cmd_volu += getc_demande(cmd, true) * cmd.get_nb_articles();
    }
    return pow(get_nb_cmd(), 2) / nb_cmd_mesured
           * (get_ratio_volu() * articles_cmd_volu
              + articles_cmd_std * (1 - get_ratio_volu()));
}

double ProblemeStochastique::getc_quantite(CommandeType const &cmd,
                                           bool volu) const {
    return Probleme::getc_quantite(cmd, volu) * nb_cmd_mesured / get_nb_cmd();
}
double ProblemeStochastique::getc_quantite_expected(CommandeType const &cmd,
                                                    bool volu) const {
    return Probleme::getc_quantite(cmd, volu);
}

int ProblemeStochastique::getc_nb_articles_mesured(bool volu) const {
    return getc_nb_articles(volu) * nb_cmd_mesured / get_nb_cmd();
}

double Probleme::get_prix_prepa_itineraire(Itineraire &itin,
                                           int i,
                                           int n,
                                           std::string lieu) const {
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
    return quantite * getc_prix_preration().at(lieu)[0]
           + quantite_volu * getc_prix_preration().at(lieu)[1];
}

double
    Probleme::get_prix_total_itineraire(Itineraire &itin, int i, int n) const {
    return itin.get_prix_livraison()
           + get_prix_prepa_itineraire(itin, i, n, "PFS")
           + get_prix_prepa_itineraire(itin, i, n, "Mag")
           + get_prix_prepa_itineraire(itin, i, n, "CAR");
}
double ProblemePrecis::get_prix_prepa_itineraire(Itineraire &itin,
                                                 int i,
                                                 int n,
                                                 std::string lieu) const {
    double price = Probleme::get_prix_prepa_itineraire(itin, i, n, lieu);
    if (itin.get_prix().contains("Mag") && itin.get_prix().at("Mag") != 0
        && lieu == "Mag") {
        if (itin.is_volumineux() && i == n && i != 0) {
            price += transitPrice * 1.1;
        } else if (i == n && i != 0) {
            price += transitPrice;
        } else if (itin.is_volumineux() && i != n && i != 0) {
            price += consolidationPrice * 1.1;
        } else if (i != n && i != 0) {
            price += consolidationPrice;
        }
    }
    return price;
}