#pragma once
#include "coin/OsiSolverInterface.hpp"
#include "lp.h"

lp::LpDecatWithStock stochastic_problem(std::string data_dir,
                                        OsiSolverInterface &solver_interface);
