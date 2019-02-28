add_executable(
        letest

        main.cpp
        prfs.cpp
        hashes.cpp
        util.cpp
        bits_and_numbers.cpp
        lifecycle.cpp
        merkle_tree_addition.cpp
        transfer.cpp
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