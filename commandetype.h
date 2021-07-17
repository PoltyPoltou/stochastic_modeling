#pragma once

class CommandeType {
  private:
    int nb_articles;
    int delai;
    int heure;

  public:
    CommandeType(int nb_articles, int heure, int delai) :
        nb_articles(nb_articles),
        delai(delai),
        heure(heure) {};

    int get_nb_articles() const { return nb_articles; }
    int get_delai() const { return delai; }
    int get_heure() const { return heure; }
};

struct Compare_CmdType {
    bool operator()(CommandeType const &rhs, CommandeType const &lhs) const {
        return rhs.get_nb_articles() < lhs.get_nb_articles()
               || (rhs.get_nb_articles() == lhs.get_nb_articles()
                   && rhs.get_delai() < lhs.get_delai())
               || (rhs.get_nb_articles() == lhs.get_nb_articles()
                   && rhs.get_delai() == lhs.get_delai()
                   && rhs.get_heure() < lhs.get_heure());
    }
};
