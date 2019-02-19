add_executable(
        letest

        main.cpp
        prfs.cpp
        hashes.cpp
        util.cpp
        note_commitment.cpp
        bits_and_numbers.cpp
        lifecycle.cpp
)
target_link_libraries(
        letest

        gtest
        ff
        snark
        zktrade
)

target_include_directories(
        letest

        PUBLIC
        ${DEPENDS_DIR}/libsnark
        ${DEPENDS_DIR}/libsnark/depends/libff
)