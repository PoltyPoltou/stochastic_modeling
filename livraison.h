#pragma once

class Livraison {
  private:
    double prix;
    int cutoff;
    int delai;

  public:
    Livraison(double prix, int cutoff, int delai) :
        prix(prix),
        cutoff(cutoff),
        delai(delai) {};

    double get_prix() { return prix; }
    int get_delai() { return delai; }
    int get_cutoff() { return cutoff; }
};
