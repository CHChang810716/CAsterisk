cmake_minimum_required(VERSION 3.10.0)
if(POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW)  # find_package() ought to search _ROOT variables
endif()
include(cmake/HunterGate.cmake)
set(
    HUNTER_CACHE_SERVERS
    "https://github.com/CHChang810716/hunter-binary-cache"
    CACHE
    STRING
    "Hunter cache servers"
)
HunterGate(
    URL "https://github.com/CHChang810716/hunter/archive/refs/tags/v0.25.6-chc-beta.tar.gz"
    SHA1 "141fafbc5d4369829f04588b8d1000ef0d8dcf88"
    FILEPATH ${CMAKE_CURRENT_LIST_DIR}/cmake/hunter_config.cmake
)
project(CppProjectTemplate
    LANGUAGES C CXX 
    VERSION 0.1.0.0
)
option(BUILD_TEST   "Build project with test"   OFF)
option(INSTALL_DEPS "Install dependensies"      OFF)
include(cmake/compiler.cmake)
include(cmake/packages.cmake)
include(cmake/src.cmake)
include(cmake/ide.cmake)
include(cmake/install.cmake)
