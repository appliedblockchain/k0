add_executable(
        letest

        bits_and_numbers.cpp
        comms.cpp
        encryption.cpp
        hashes.cpp
        json.cpp
        ka.cpp
        lifecycle.cpp
        main.cpp
        merkle_tree.cpp
        merkle_tree_addition.cpp
        prfs.cpp
        summation.cpp
        transfer.cpp
        util.cpp
)
target_link_libraries(
        letest

        ff
        snark
        k0
        jsoncpp
        sodium
        ${GTEST_LDFLAGS}
)
include_directories(../src)
target_compile_options(letest PUBLIC ${GTEST_CFLAGS})
