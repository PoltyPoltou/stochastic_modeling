#pragma once

#include <string>

void testall(std::string data_dir);

void testCsv(std::string data_dir);
void testCplex();
void testLpInterface();
void testLpCut();
void testReadRouteCsv(std::string data_dir);
void testLoadDataLp(std::string data_dir);
void testStochastic(std::string data_dir);
void testPbPrecis(std::string data_dir);