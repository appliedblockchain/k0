add_executable(
        letest

        bits_and_numbers.cpp
        comms.cpp
        hashes.cpp
        lifecycle.cpp
        main.cpp
        merkle_tree.cpp
        merkle_tree_addition.cpp
        prfs.cpp
        transfer.cpp
        util.cpp
)
target_link_libraries(
        letest

        ff
        snark
        zktrade
        sodium
        ${GTEST_LDFLAGS}
)


target_include_directories(
        letest

        PUBLIC
        ${DEPENDS_DIR}/libsnark
        ${DEPENDS_DIR}/libsnark/depends/libff
)

target_compile_options(letest PUBLIC ${GTEST_CFLAGS})