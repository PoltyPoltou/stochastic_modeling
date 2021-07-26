#pragma once
#include "commandetype.h"
#include "const.h"
#include "itineraire.h"
#include "livraison.h"
#include <map>
#include <set>
#include <string>
#include <vector>
// Business object input
class Probleme {

  private:
    int nb_cmd;
    double ratio_volu;
    std::vector<Itineraire> vec_itineraires;
    std::vector<Livraison> vec_livraison_volu_mag;
    std::vector<Livraison> vec_livraison_volu_pfs;
    Livraison livraison_car;
    // first key is the place,
    // second is 0 for standard 1 for volu
    std::map<std::string, std::array<double, 2>> stocks;
    // first key is the place,
    // second is 0 for standard 1 for volu
    std::map<std::string, std::array<double, 2>> prix_preration;
    // first key is the place,
    // second is 0 for standard 1 for volu
    std::map<CommandeType, std::array<double, 2>, Compare_CmdType> demande;
    std::map<CommandeType, std::array<double, 2>, Compare_CmdType> quantite;

  public:
    static const int articles_max = 9;
    static const int delai_min = 1, delai_max = 3;
    static std::set<CommandeType, Compare_CmdType> commandes_set;

    Probleme(int n_cmd, double ratio_volumineux, Livraison car);
    Probleme(int n_cmd,
             double ratio_volumineux,
             Livraison car,
             std::array<double, 2> stock_pfs,
             std::array<double, 2> stock_mag,
             std::array<double, 2> stock_car,
             std::array<double, 2> prix_prepa_pfs,
             std::array<double, 2> prix_prepa_mag,
             std::array<double, 2> prix_prepa_car);

    int get_nb_cmd() const { return nb_cmd; };
    double get_ratio_volu() const { return ratio_volu; };

    Livraison get_livraison_car() { return livraison_car; };

    std::vector<Itineraire> &get_vec_itineraires() { return vec_itineraires; };
    std::vector<Itineraire> const &getc_vec_itineraires() const {
        return vec_itineraires;
    };

    std::vector<Livraison> &get_vec_livraison_volu_mag() {
        return vec_livraison_volu_mag;
    };

    std::vector<Livraison> &get_vec_livraison_volu_pfs() {
        return vec_livraison_volu_pfs;
    };

    std::map<std::string, std::array<double, 2>> &get_stocks() {
        return stocks;
    };
    std::map<std::string, std::array<double, 2>> const &getc_stocks() const {
        return stocks;
    };

    std::map<std::string, std::array<double, 2>> &get_prix_preration() {
        return prix_preration;
    };
    std::map<std::string, std::array<double, 2>> const &
        getc_prix_preration() const {
        return prix_preration;
    };

    double getc_demande(CommandeType const &cmd, bool volu) const {
        return demande.at(cmd).at(volu);
    };

    void set_demande(CommandeType const &cmd, double std, double volu);
    void compute_quantite(CommandeType const &cmd);

    double getc_quantite(CommandeType const &cmd, bool volu = false) const;
    int getc_nb_articles(bool volu = false) const;
};

double get_prix_total_itineraire(Probleme const &pb,
                                 Itineraire &itin,
                                 int i,
                                 int n);
double get_prix_prepa_itineraire(
    Probleme const &pb, Itineraire &itin, int i, int n, std::string lieu);