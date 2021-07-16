#pragma once
#include <array>
#include <map>
#include <string>

struct Itineraire {
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
               std::string depart_volu,
               std::array<bool, 3> possibles,
               std::map<std::string, double> prix) :
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
