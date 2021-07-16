#include "coin_util.h"

#include "coin/CoinPackedVector.hpp"

#include <vector>

CoinPackedVector std_to_coin_vector(std::vector<double> &vec) {
    CoinPackedVector coin_vec;
    for (int i = 0; i < vec.size(); i++) {
        coin_vec.insert(i, vec[i]);
    }
    return coin_vec;
}
