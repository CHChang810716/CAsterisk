set(AKT_VSCODE_GCC_TASK_TMPL ${CMAKE_SOURCE_DIR}/src/catkc.launch_gdb.in)
include(cmake/scheme/exe.cmake)
target_link_libraries(${AKT_TARGET} PRIVATE catk-setting catk-symdb catk-analysis)