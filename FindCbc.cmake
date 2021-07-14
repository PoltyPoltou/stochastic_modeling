set (CBC_INCLUDE_DIRS /usr/include/coin)
set (CBC_LIBRARY_DIRS /usr/lib/x86-64-linux-gnu )

set (CBC_LIBRARIES)

set (libs "CbcSolver" "Cbc" "Cgl" "Clp" "CoinUtils" "OsiCbc" "OsiClp" "Osi")
foreach (l ${libs})
  find_library(
    ${l}_LIBRARY
    NAMES
    ${l}
    lib${l}
    PATHS
    ${CBC_LIBRARY_DIRS})

  list (APPEND CBC_LIBRARIES ${${l}_LIBRARY})
endforeach ()

include (FindPackageHandleStandardArgs)
find_package_handle_standard_args (Cbc REQUIRED_VARS CBC_LIBRARIES Cbc_LIBRARY CbcSolver_LIBRARY Cgl_LIBRARY Clp_LIBRARY CoinUtils_LIBRARY OsiCbc_LIBRARY OsiClp_LIBRARY Osi_LIBRARY CBC_INCLUDE_DIRS)

if (0)
  find_package(lapack REQUIRED)
endif ()

if (0)
  find_package(ZLIB REQUIRED)
endif ()

if (0)
  find_package(BZIP2 REQUIRED)
endif ()

if (1)
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads)
endif ()

if (NOT TARGET COIN::Cbc)
  add_library (COIN::Cbc INTERFACE IMPORTED)
  set_property (TARGET COIN::Cbc PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CBC_INCLUDE_DIRS})
  set_property (TARGET COIN::Cbc PROPERTY INTERFACE_LINK_LIBRARIES ${CBC_LIBRARIES})
  if (0)
    set_property (TARGET COIN::Cbc APPEND PROPERTY INTERFACE_LINK_LIBRARIES lapack)
  endif ()
  if (0)
    set_property (TARGET COIN::Cbc APPEND PROPERTY INTERFACE_LINK_LIBRARIES ZLIB::ZLIB)
  endif ()
  if (0)
    set_property (TARGET COIN::Cbc APPEND PROPERTY INTERFACE_LINK_LIBRARIES BZIP2::BZIP2)
  endif ()
  if (0)
    set_property (TARGET COIN::Cbc APPEND PROPERTY INTERFACE_LINK_LIBRARIES gfortran)
  endif ()
  if (1)
    set_property (TARGET COIN::Cbc APPEND PROPERTY INTERFACE_LINK_LIBRARIES Threads::Threads)
  endif ()
  if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set_property (TARGET COIN::Cbc APPEND PROPERTY INTERFACE_LINK_LIBRARIES rt)
  endif ()
  #set_property (TARGET COIN::Cbc PROPERTY INTERFACE_COMPILE_DEFINITIONS)
endif()

