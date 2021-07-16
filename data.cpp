#include "data.h"
#include "csv.h"
#include <iostream>

void read_and_gen_data_from_csv(Probleme &probleme,
                                std::string data_folder_path) {
    load_std_itineraires(probleme, data_folder_path);
    std::map<int, double> delta_volu_map(
        load_delta_livraison_volu(probleme, data_folder_path));
    load_livraison_pfs_volu(probleme, data_folder_path);
    load_livraison_mag_volu(probleme, data_folder_path);
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

void generate_livraison_volu(Probleme &probleme, std::string data_folder_path) {
}