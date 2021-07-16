#include "itineraire.h"

std::ostream &operator<<(std::ostream &os, Itineraire &itin) {
    bool volu = itin.is_volumineux();
    std::array<bool, 3> &possibles = itin.get_possibles();
    if (!volu) {
        if (possibles[0]) {
            os << "Mag--->Client";
            return os;
        } else if (possibles[1]) {
            os << "PFS--->Mag--->Client";
            return os;
        } else {
            os << "PFS--->Client";
            return os;
        }
    }
    return os;
}

double Itineraire::get_prix_livraison() {
    double somme = 0;
    for (auto iter : prix) {
        somme += iter.second;
    }
    return somme;
}