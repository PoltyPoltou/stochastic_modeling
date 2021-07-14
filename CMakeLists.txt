cmake_minimum_required(VERSION 3.16)

set(CMAKE_MODULE_PATH /home/polty/document/cpp)
set(CMAKE_CXX_STANDARD 20)

project(stochastic_decathlon)

find_package(Cbc REQUIRED)

add_executable(main_file "main.cpp")
target_link_libraries(main_file COIN::Cbc)