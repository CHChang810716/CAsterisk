set(AKT_SRC_ROOT_DIR "${CMAKE_SOURCE_DIR}/src")
if(BUILD_TEST)
    enable_testing()
endif()
if(NOT BUILD_TEST)
    akt_exclude_targets_in(${CMAKE_SOURCE_DIR}/src/test)
endif()
unset(AKT_IMPORTED_TARGETS)
akt_set_all_imported_targets()
akt_show_var(AKT_IMPORTED_TARGETS)
akt_src_based_add_targets_in(${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/cmake/scheme/exe.cmake)