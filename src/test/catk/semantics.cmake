include(cmake/scheme/test.cmake)
foreach(child_src ${AKT_CHILD_SRCS})
  target_sources(${AKT_TARGET} PRIVATE ${child_src})
endforeach()
target_link_libraries(${AKT_TARGET} PRIVATE 
  catk-setting catk-semantics
)