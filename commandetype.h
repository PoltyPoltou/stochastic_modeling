#pragma once

class CommandeType {
  private:
    int nb_articles;
    int delai;
    int heure;

  public:
    CommandeType(int nb_articles, int delai, int heure) :
        nb_articles(nb_articles),
        delai(delai),
        heure(heure) {};

    int get_nb_articles() { return nb_articles; }
    int get_delai() { return delai; }
    int get_heure() { return heure; }
};
