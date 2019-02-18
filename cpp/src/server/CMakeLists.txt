add_executable(
        server

        run_server.cpp
)
target_link_libraries(
        server

        snark
        zktrade
        jsoncpp
        microhttpd
        jsonrpccpp-common
        jsonrpccpp-server
)
target_include_directories(
        server

        PUBLIC
        ${DEPENDS_DIR}/libsnark
        ${DEPENDS_DIR}/libsnark/depends/libff
        ${DEPENDS_DIR}/libsnark/depends/libfqfft
)
add_executable(
        merkle_tree_inclusion_test

        inclusion_test.cpp
        ../MerkleTree.cpp
        ../serialization.cpp
)
target_link_libraries(
        merkle_tree_inclusion_test

        snark
)
target_include_directories(
        merkle_tree_inclusion_test

        PUBLIC
        ${DEPENDS_DIR}/libsnark
        ${DEPENDS_DIR}/libsnark/depends/libff
        ${DEPENDS_DIR}/libsnark/depends/libfqfft
)