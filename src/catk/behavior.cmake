include(cmake/scheme/lib.cmake)
foreach(child_src ${AKT_CHILD_SRCS})
  akt_show_var(child_src)
  target_sources(${AKT_TARGET} PRIVATE ${child_src})
endforeach()
target_link_libraries(${AKT_TARGET} PRIVATE 
  catk-io catk-utils catk-semantics catk-type ${llvm_libs}
)