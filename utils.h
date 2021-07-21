#pragma once
#include <array>
#include <map>
#include <ostream>

template<size_t Nm>
std::ostream &operator<<(std::ostream &os,
                         std::array<double, Nm> const &array) {
    os << "{ ";
    for (size_t i = 0; i < Nm - 1; i++) {
        os << array.at(i) << ", ";
    }
    os << array.at(Nm - 1) << " }";
    return os;
}

template<size_t Nm>
std::ostream &
    operator<<(std::ostream &os,
               std::map<std::string, std::array<double, Nm>> const &map) {
    os << "{ ";
    for (auto iter : map) {
        os << iter.first << " : " << iter.second << ", ";
    }
    os << "}";
    return os;
}

std::ostream &operator<<(std::ostream &os,
                         std::map<std::string, double> const &map) {
    os << "{ ";
    for (auto iter : map) {
        os << iter.first << " : " << iter.second << ", ";
    }
    os << "}";
    return os;
}
