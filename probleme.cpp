#include "probleme.h"

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
    nb_cmd_stochastic(n_cmd),
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
        articles_cmd_std +=
            getc_demande().at(cmd)[false] * cmd.get_nb_articles();
        articles_cmd_volu +=
            getc_demande().at(cmd)[true] * cmd.get_nb_articles();
    }
    return nb_cmd
           * (ratio_volu * articles_cmd_volu
              + articles_cmd_std * (1 - ratio_volu));
}

void Probleme::set_demande(CommandeType const &cmd, double std, double volu) {
    demande[cmd] = {std, volu};
    compute_quantite(cmd);
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

double get_prix_total_itineraire(Probleme const &pb,
                                 Itineraire &itin,
                                 int i,
                                 int n) {
    return itin.get_prix_livraison()
           + get_prix_prepa_itineraire(pb, itin, i, n, "PFS")
           + get_prix_prepa_itineraire(pb, itin, i, n, "Mag")
           + get_prix_prepa_itineraire(pb, itin, i, n, "CAR");
}