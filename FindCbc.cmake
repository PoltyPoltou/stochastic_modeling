set (CBC_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/include)
set (CBC_LIBRARY_DIRS ${CMAKE_CURRENT_LIST_DIR}/lib)

set (CBC_LIBRARIES)

set (libs "CoinUtils" "OsiCpx" "Osi")
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
find_package_handle_standard_args (Cbc REQUIRED_VARS CBC_LIBRARIES  CoinUtils_LIBRARY OsiCpx_LIBRARY Osi_LIBRARY CBC_INCLUDE_DIRS)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)


if (NOT TARGET COIN::Cbc)
  add_library (COIN::Cbc INTERFACE IMPORTED)
  set_property (TARGET COIN::Cbc PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${CBC_INCLUDE_DIRS})
  set_property (TARGET COIN::Cbc PROPERTY INTERFACE_LINK_LIBRARIES ${CBC_LIBRARIES})
  set_property (TARGET COIN::Cbc APPEND PROPERTY INTERFACE_LINK_LIBRARIES Threads::Threads)
  if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set_property (TARGET COIN::Cbc APPEND PROPERTY INTERFACE_LINK_LIBRARIES rt)
  endif ()
endif()

