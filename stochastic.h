#pragma once
#include "coin/OsiSolverInterface.hpp"
#include "lp.h"
#include "probleme.h"

void stochastic_problem(std::string data_dir);
void benders_decomposition(std::string data_dir);