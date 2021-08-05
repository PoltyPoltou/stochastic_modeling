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

    std::map<std::string, std::array<double, 2>> const &getc_stocks() const {
        return stocks;
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

    virtual double getc_quantite(CommandeType const &cmd,
                                 bool volu = false) const;
    virtual int getc_nb_articles(bool volu = false) const;
    double get_prix_total_itineraire(Itineraire &itin, int i, int n) const;
    virtual double get_prix_prepa_itineraire(Itineraire &itin,
                                             int i,
                                             int n,
                                             std::string lieu) const;
};

class ProblemePrecis : public Probleme {
  private:
    double consolidationPrice, transitPrice;

  public:
    ProblemePrecis(int n_cmd,
                   double ratio_volumineux,
                   Livraison car,
                   double employee_price_hour,
                   double time_minutes_consolidation,
                   double time_minutes_transit) :
        Probleme(n_cmd, ratio_volumineux, car),
        consolidationPrice(employee_price_hour * time_minutes_consolidation
                           / 60),
        transitPrice(employee_price_hour * time_minutes_transit / 60) {};
    virtual double get_prix_prepa_itineraire(Itineraire &itin,
                                             int i,
                                             int n,
                                             std::string lieu) const;
};

class ProblemeStochastique : public Probleme {
  private:
    int nb_cmd_mesured;

  public:
    ProblemeStochastique(int n_cmd, double ratio_volumineux, Livraison car) :
        Probleme(n_cmd, ratio_volumineux, car),
        nb_cmd_mesured(n_cmd) {};

    int get_nb_cmd_mesured() const { return nb_cmd_mesured; };
    void set_nb_cmd_mesured(int n) { nb_cmd_mesured = n; };
    double getc_quantite_expected(CommandeType const &cmd,
                                  bool volu = false) const;
    virtual double getc_quantite(CommandeType const &cmd,
                                 bool volu = false) const;
    int getc_nb_articles_mesured(bool volu = false) const;
    virtual int getc_nb_articles(bool volu = false) const;
};
