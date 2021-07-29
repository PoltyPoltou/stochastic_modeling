#pragma once
#include "lp.h"
#include <vector>

void benders_decomposition(lp::LinearInterface main_lp,
                           std::vector<lp::LinearInterface> vec_follower_lp,
                           Probleme baseProblem);