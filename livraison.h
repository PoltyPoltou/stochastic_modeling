#pragma once

class Livraison {
  private:
    double prix;
    int delai;
    int cutoff;

  public:
    Livraison(double prix, int delai, int cutoff) :
        prix(prix),
        delai(delai),
        cutoff(cutoff) {};

    double get_prix() { return prix; }
    int get_delai() { return delai; }
    int get_cutoff() { return cutoff; }
};
