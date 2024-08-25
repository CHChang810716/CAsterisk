set(AKT_VSCODE_GCC_TASK_TMPL ${CMAKE_CURRENT_LIST_DIR}/../vscode/lanuch_gdb.in)
include(${CMAKE_CURRENT_LIST_DIR}/exe.cmake)
target_link_libraries(${AKT_TARGET} PRIVATE GTest::gtest_main)
target_compile_definitions(${AKT_TARGET} PRIVATE UNIT_TEST)
add_test(
    NAME ${AKT_TARGET}
    COMMAND ${CMAKE_INSTALL_PREFIX}/bin/${AKT_TARGET}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/workdir 
)