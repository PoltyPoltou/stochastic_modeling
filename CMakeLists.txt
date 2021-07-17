cmake_minimum_required(VERSION 3.16)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_CXX_STANDARD 20)

project(stochastic_decathlon)

find_package(Cbc REQUIRED)
add_executable(main_file main.cpp)

target_sources(main_file PRIVATE
csv.cpp
csv.h
coin_util.cpp
coin_util.h
test.cpp
test.h
data.cpp
data.h
probleme.cpp
probleme.h
itineraire.cpp
itineraire.h
livraison.h
commandetype.h
)
target_include_directories(main_file PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(main_file COIN::Cbc)