#pragma once
#include "probleme.h"
#include <map>
#include <string>

void read_and_gen_data_from_csv(Probleme &probleme,
                                std::string data_folder_path);

void load_std_itineraires(Probleme &probleme, std::string data_folder_path);

std::map<int, double> load_delta_livraison_volu(Probleme &probleme,
                                                std::string data_folder_path);

void load_livraison_pfs_volu(Probleme &probleme, std::string data_folder_path);

void load_livraison_mag_volu(Probleme &probleme, std::string data_folder_path);

void generate_livraison_volu(Probleme &probleme,
                             std::string data_folder_path,
                             std::map<int, double> delta_volu_map);

void generate_demand(Probleme probleme);