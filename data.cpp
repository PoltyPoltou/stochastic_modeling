#include "data.h"
#include "commandetype.h"
#include "csv.h"
#include <iostream>
#include <math.h>
void read_and_gen_data_from_csv(Probleme &probleme,
                                std::string data_folder_path) {
    load_std_itineraires(probleme, data_folder_path);
    std::map<int, double> delta_volu_map(
        load_delta_livraison_volu(probleme, data_folder_path));
    load_livraison_pfs_volu(probleme, data_folder_path);
    load_livraison_mag_volu(probleme, data_folder_path);
    generate_livraison_volu(probleme, data_folder_path, delta_volu_map);
    generate_demand(probleme);
}

void load_std_itineraires(Probleme &probleme, std::string data_folder_path) {
    std::vector<double> itineraire_vec;
    std::array<bool, 3> possibles;
    std::map<std::string, double> prix;
    csv::CsvFileStream file(data_folder_path + "route.csv");
    file.open();
    csv::skip_line(file); // discarding header line
    while (!file.eof()) {
        csv::get_line(file, itineraire_vec);

        prix["PFS"] = itineraire_vec[0];
        prix["Mag"] = itineraire_vec[1];
        possibles[0] = itineraire_vec[5];
        possibles[1] = itineraire_vec[6];
        possibles[2] = itineraire_vec[7];
        probleme.get_vec_itineraires().push_back(Itineraire(
            itineraire_vec[4], itineraire_vec[3], false, possibles, prix));
        itineraire_vec.clear();
    }
    file.close();
}

// key is the whole part of std price mutliplied by 100
std::map<int, double> load_delta_livraison_volu(Probleme &probleme,
                                                std::string data_folder_path) {
    std::map<int, double> delta_volu_map;
    std::vector<double> buffer_vec;
    csv::CsvFileStream file(data_folder_path + "delta_volu.csv");
    file.open();
    csv::skip_line(file); // discarding header line
    while (!file.eof()) {
        csv::get_line(file, buffer_vec);
        if (buffer_vec[2] != -1) {
            delta_volu_map[100 * buffer_vec[0]] = buffer_vec[2];
        }
        buffer_vec.clear();
    }
    file.close();
    return delta_volu_map;
}

void load_livraison_pfs_volu(Probleme &probleme, std::string data_folder_path) {
    std::vector<double> buffer_vec;
    csv::CsvFileStream file(data_folder_path + "livraison_volu_pfs.csv");
    file.open();
    csv::skip_line(file); // discarding header line
    while (!file.eof()) {
        csv::get_line(file, buffer_vec);
        probleme.get_vec_livraison_volu_pfs().push_back(
            Livraison(buffer_vec[0], buffer_vec[1], buffer_vec[2]));
        buffer_vec.clear();
    }
    file.close();
}

void load_livraison_mag_volu(Probleme &probleme, std::string data_folder_path) {
    std::vector<double> buffer_vec;
    csv::CsvFileStream file(data_folder_path + "livraison_volu_mag.csv");
    file.open();
    csv::skip_line(file); // discarding header line
    while (!file.eof()) {
        csv::get_line(file, buffer_vec);
        probleme.get_vec_livraison_volu_mag().push_back(
            Livraison(buffer_vec[0], buffer_vec[1], buffer_vec[2]));
        buffer_vec.clear();
    }
    file.close();
}

// delta_volu_map : key is the whole part of std price mutliplied by 100
void generate_livraison_volu(Probleme &probleme,
                             std::string data_folder_path,
                             std::map<int, double> delta_volu_map) {
    for (Itineraire itin :
         std::vector<Itineraire>(probleme.get_vec_itineraires())) {

        int pfs_price_key = floor(100 * itin.get_prix()["PFS"]);
        int mag_price_key = floor(100 * itin.get_prix()["Mag"]);

        if (itin.get_possibles()[0]) { // Magasin only
            // PFS
            Itineraire new_itin(itin);
            new_itin.set_volumineux(true);
            new_itin.set_depart_volu("PFS");
            if (delta_volu_map.contains(mag_price_key)) {
                new_itin.get_prix()["Mag"] += delta_volu_map[mag_price_key];
                for (Livraison l : probleme.get_vec_livraison_volu_pfs()) {
                    Itineraire itin_pfs(new_itin);
                    itin_pfs.get_prix()["PFS"] = l.get_prix();
                    itin_pfs.set_delai(itin_pfs.get_delai() + l.get_delai());
                    itin_pfs.set_cutoff(l.get_cutoff());
                    probleme.get_vec_itineraires().push_back(itin_pfs);
                }
            }

            // Mag
            new_itin = Itineraire(itin);
            new_itin.set_volumineux(true);
            new_itin.set_depart_volu("Mag");
            if (delta_volu_map.contains(mag_price_key)) {
                new_itin.get_prix()["Mag"] += delta_volu_map[mag_price_key];
                probleme.get_vec_itineraires().push_back(new_itin);
            }

            // CAR
            new_itin = Itineraire(itin);
            new_itin.set_volumineux(true);
            new_itin.set_depart_volu("CAR");
            if (delta_volu_map.contains(mag_price_key)) {
                new_itin.get_prix()["Mag"] += delta_volu_map[mag_price_key];
                new_itin.get_prix()["CAR"] =
                    probleme.get_livraison_car().get_prix();
                new_itin.set_delai(new_itin.get_delai()
                                   + probleme.get_livraison_car().get_delai());
                new_itin.set_cutoff(probleme.get_livraison_car().get_cutoff());
                probleme.get_vec_itineraires().push_back(new_itin);
            }
        } else if (itin.get_possibles()[1]) { // Mag + PFS
            // PFS
            Itineraire new_itin(itin);
            new_itin.set_volumineux(true);
            new_itin.set_depart_volu("PFS");
            if (delta_volu_map.contains(mag_price_key)
                && delta_volu_map.contains(pfs_price_key)) {
                new_itin.get_prix()["Mag"] += delta_volu_map[mag_price_key];
                new_itin.get_prix()["PFS"] += delta_volu_map[pfs_price_key];
                probleme.get_vec_itineraires().push_back(new_itin);
            }

            // Mag
            new_itin = Itineraire(itin);
            new_itin.set_volumineux(true);
            new_itin.set_depart_volu("Mag");
            if (delta_volu_map.contains(mag_price_key)) {
                new_itin.get_prix()["Mag"] += delta_volu_map[mag_price_key];
                new_itin.get_possibles()[2] = true;
                probleme.get_vec_itineraires().push_back(new_itin);
            }
            // CAR
            new_itin = Itineraire(itin);
            new_itin.set_volumineux(true);
            new_itin.set_depart_volu("CAR");
            if (delta_volu_map.contains(mag_price_key)) {
                new_itin.get_prix()["Mag"] += delta_volu_map[mag_price_key];
                new_itin.get_prix()["CAR"] =
                    probleme.get_livraison_car().get_prix();
                new_itin.get_possibles()[2] = true;
                new_itin.set_cutoff(
                    new_itin.get_cutoff() == 14 ?
                        std::min(new_itin.get_cutoff(),
                                 probleme.get_livraison_car().get_cutoff()) :
                        new_itin.get_cutoff());
                probleme.get_vec_itineraires().push_back(new_itin);
            }
        } else { // PFS only
            // PFS
            Itineraire new_itin(itin);
            new_itin.set_volumineux(true);
            new_itin.set_depart_volu("PFS");
            if (delta_volu_map.contains(pfs_price_key)) {
                new_itin.get_prix()["PFS"] += delta_volu_map[pfs_price_key];
                probleme.get_vec_itineraires().push_back(new_itin);
            }
            // Mag
            new_itin = Itineraire(itin);
            new_itin.set_volumineux(true);
            new_itin.set_depart_volu("Mag");
            for (Livraison l : probleme.get_vec_livraison_volu_pfs()) {
                Itineraire itin_pfs(new_itin);
                itin_pfs.get_prix()["Mag"] = l.get_prix();
                itin_pfs.set_delai(
                    std::max(itin_pfs.get_delai(), l.get_delai()));
                itin_pfs.set_cutoff(
                    std::min(itin_pfs.get_cutoff(), l.get_cutoff()));
                probleme.get_vec_itineraires().push_back(itin_pfs);
            }
        }
    }
}

void generate_demand(Probleme &probleme) {
    for (CommandeType cmd : Probleme::commandes_set) {
        probleme.set_demande(cmd, 1. / Probleme::commandes_set.size(),
                             1. / Probleme::commandes_set.size());
    }
}