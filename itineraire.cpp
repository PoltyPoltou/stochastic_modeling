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
    } else {
        if (itin.get_possibles()[0]) {
            if (itin.get_depart_volu() == "Mag") {
                os << "Mag-V->Client";
            }
            if (itin.get_depart_volu() == "PFS") {
                os << "PFS-V->Mag-V->Client";
            }
            if (itin.get_depart_volu() == "CAR") {
                os << "CAR-V->Mag-V->Client";
            }
        } else if (itin.get_possibles()[1]) {
            if (itin.get_depart_volu() == "Mag") {
                os << "PFS--->Mag-V->Client";
            }
            if (itin.get_depart_volu() == "PFS") {
                os << "PFS-V->Mag-V->Client";
            }
            if (itin.get_depart_volu() == "CAR") {
                os << "\nPFS---|\n      |->Mag-V->Client \nCAR-V-|";
            }
        } else { // itin.get_possibles()[2]
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
    return os;
}

double Itineraire::get_prix_livraison() {
    double somme = 0;
    for (auto iter : prix) {
        somme += iter.second;
    }
    return somme;
}