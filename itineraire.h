#pragma once
#include <array>
#include <iostream>
#include <map>
#include <string>

class Itineraire {
  private:
    int delai;
    int cutoff;
    bool volumineux;
    std::string depart_volu;
    std::array<bool, 3> possibles;
    std::map<std::string, double> prix;

  public:
    Itineraire(int delai,
               int cutoff,
               bool volumineux,
               std::array<bool, 3> possibles,
               std::map<std::string, double> prix,
               std::string depart_volu = "") :
        delai(delai),
        cutoff(cutoff),
        volumineux(volumineux),
        depart_volu(depart_volu),
        possibles(possibles),
        prix(prix) {};

    int get_delai() const { return delai; }
    int get_cutoff() const { return cutoff; }
    bool is_volumineux() const { return volumineux; }
    std::string get_depart_volu() const { return depart_volu; }
    std::array<bool, 3> &get_possibles() { return possibles; }
    std::map<std::string, double> &get_prix() { return prix; }
    void set_delai(int new_delai) { delai = new_delai; };
    void set_cutoff(int new_cutoff) { cutoff = new_cutoff; };
    void set_volumineux(bool new_volumineux) { volumineux = new_volumineux; };
    void set_depart_volu(std::string new_depart_volu) {
        depart_volu = new_depart_volu;
    };

    double get_prix_livraison();
};

std::ostream &operator<<(std::ostream &os, Itineraire &itin);