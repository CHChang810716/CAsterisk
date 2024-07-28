set(lc_srcs ${AKT_CHILD_SRCS})
set(target_prefix ${AKT_TARGET})
foreach(lc_src ${lc_srcs})
  file(RELATIVE_PATH rel_lc_src ${CMAKE_CURRENT_LIST_DIR}/semantics ${lc_src})
  akt_target_name(lc_target_name "${rel_lc_src}")
  set(AKT_TARGET ${target_prefix}-${lc_target_name})
  set(AKT_CHILD_SRCS ${lc_src})
  set(AKT_CHILD_TARGETS)
  include(cmake/scheme/test.cmake)
  target_link_libraries(${AKT_TARGET} PRIVATE 
    catk-setting catk-semantics
  )
endforeach()