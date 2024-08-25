hunter_add_package(Arkitekto)
find_package(Arkitekto CONFIG REQUIRED)
if(BUILD_TEST)
    hunter_add_package(GTest)
    find_package(GTest CONFIG REQUIRED)
    akt_imported_targets_exclude(GTest::gtest_main GTest::gmock_main)
endif()
find_package(Threads REQUIRED)

hunter_add_package(LLVM)
find_package(LLVM CONFIG REQUIRED)
llvm_map_components_to_libnames(llvm_libs support core)

hunter_add_package(pegtl)
find_package(pegtl CONFIG REQUIRED)

hunter_add_package(Boost COMPONENTS 
    system
    filesystem
)
find_package(Boost CONFIG COMPONENTS
    system
    filesystem
    REQUIRED
)

hunter_add_package(fmt)
find_package(fmt CONFIG COMPONENTS)

hunter_add_package(range-v3)
find_package(range-v3)