#include <iostream>
#include "coin/OsiClpSolverInterface.hpp"
int main(int argc, char const *argv[])
{
    OsiSolverInterface *test = new OsiClpSolverInterface;
    delete test;
    std::cout << "azd" << std::endl;
    return 0;
}
