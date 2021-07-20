#include "itineraire.h"

std::ostream &operator<<(std::ostream &os, Itineraire const &itin) {
    bool volu = itin.is_volumineux();
    std::array<bool, 3> const &possibles = itin.getc_possibles();
    if (!volu) {
        if (possibles[0]) {
            os << "Mag--->Client";
        } else if (possibles[1]) {
            os << "PFS--->Mag--->Client";
        } else {
            os << "PFS--->Client";
        }
    } else {
        if (possibles[0]) {
            if (itin.get_depart_volu() == "Mag") {
                os << "Mag-V->Client";
            }
            if (itin.get_depart_volu() == "PFS") {
                os << "PFS-V->Mag-V->Client";
            }
            if (itin.get_depart_volu() == "CAR") {
                os << "CAR-V->Mag-V->Client";
            }
        } else if (possibles[1]) {
            if (itin.get_depart_volu() == "Mag") {
                os << "PFS--->Mag-V->Client";
            }
            if (itin.get_depart_volu() == "PFS") {
                os << "PFS-V->Mag-V->Client";
            }
            if (itin.get_depart_volu() == "CAR") {
                os << "\nPFS---|\n      |->Mag-V->Client \nCAR-V-|";
            }
        } else { // possibles[2]
            if (itin.get_depart_volu() == "Mag") {
                os << "Mag-V->Client + PFS--->Client";
            }
            if (itin.get_depart_volu() == "PFS") {
                os << "PFS-V->Client";
            }
            if (itin.get_depart_volu() == "CAR") {
                os << "CAR-V->Mag-V->Client + PFS--->Client";
            }
        }
    }
    os << " cutoff : " << itin.get_cutoff() << ", delai : " << itin.get_delai()
       << ", prix : " << itin.get_prix_livraison();
    return os;
}

double Itineraire::get_prix_livraison() const {
    double somme = 0;
    for (auto iter : prix) {
        somme += iter.second;
    }
    return somme;
}

bool Itineraire::is_possible(int i, int n, std::string lieu) {
    if (volumineux && lieu != depart_volu) {
        return false;
    }
    if (i == 0) {
        return possibles[0];
    }
    if (i == n) {
        return possibles[2];
    }
    return possibles[1];
}
