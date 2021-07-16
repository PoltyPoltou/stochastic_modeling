#pragma once
#include "commandetype.h"
#include "itineraire.h"
#include "livraison.h"
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
    std::map<CommandeType, std::array<double, 2>> demande;

  public:
    Probleme(int n_cmd, double ratio_volumineux, Livraison car);

    int get_nb_cmd() { return nb_cmd; };

    double get_ratio_volu() { return ratio_volu; };

    Livraison get_livraison_car() { return livraison_car; };

    std::vector<Itineraire> &get_vec_itineraires() { return vec_itineraires; };

    std::vector<Livraison> &get_vec_livraison_volu_mag() {
        return vec_livraison_volu_mag;
    };

    std::vector<Livraison> &get_vec_livraison_volu_pfs() {
        return vec_livraison_volu_pfs;
    };

    std::map<std::string, std::array<double, 2>> &get_stocks() {
        return stocks;
    };

    std::map<std::string, std::array<double, 2>> &get_prix_preration() {
        return prix_preration;
    };

    std::map<CommandeType, std::array<double, 2>> &get_demande() {
        return demande;
    };
};
