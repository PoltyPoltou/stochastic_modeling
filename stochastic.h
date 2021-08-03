#pragma once
#include "coin/OsiSolverInterface.hpp"
#include "lp.h"
#include "probleme.h"

lp::LpDecatScenarios stochastic_problem(std::string data_dir,
                                        OsiSolverInterface &solver_interface,
                                        ProblemeStochastique &pb_loaded);
void benders_decomposition(std::string data_dir);