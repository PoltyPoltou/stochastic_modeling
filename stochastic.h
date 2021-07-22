#pragma once
#include "coin/OsiSolverInterface.hpp"
#include "lp.h"

lp::LinearProblem stochastic_problem(std::string data_dir,
                                     OsiSolverInterface &solver_interface);
