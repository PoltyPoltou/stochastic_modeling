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

    int get_delai() { return delai; }
    int get_cutoff() { return cutoff; }
    bool get_volumineux() { return volumineux; }
    std::string get_depart_volu() { return depart_volu; }
    std::array<bool, 3> &get_possibles() { return possibles; }
    std::map<std::string, double> &get_prix() { return prix; }
};

std::ostream &operator<<(std::ostream &os, Itineraire &itin);