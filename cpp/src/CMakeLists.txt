add_executable(
        convert_vk

        convert_vk.cpp
)
target_link_libraries(
        convert_vk

        snark
)
target_include_directories(
        convert_vk

        PUBLIC
        ${DEPENDS_DIR}/libsnark
        ${DEPENDS_DIR}/libsnark/depends/libfqfft
)

add_executable(
        pack_256_bits

        pack_256_bits.cpp
)
target_link_libraries(
        pack_256_bits

        snark
)
target_include_directories(
        pack_256_bits

        PUBLIC
        ${DEPENDS_DIR}/libsnark
        ${DEPENDS_DIR}/libsnark/depends/libfqfft
)

add_library(zktrade "")

target_sources(
        zktrade

        PRIVATE
        crypto/sha256.cpp
        hash/sha256.cpp
        hash/sha256_compression.cpp
        scheme/comms.cpp
        scheme/prfs.cpp
        util.cpp
        MerkleTree.cpp
        serialization.cpp
        json_conversion.cpp

        PUBLIC
        circuitry/CommitmentCircuit.hpp
        circuitry/CommitmentCircuit.tcc
        hash/sha256.hpp
        hash/sha256_compression.hpp
        scheme/comms.hpp
        scheme/prfs.h
        server/Server.hpp
        server/Server.tcc
        util.h
        MerkleTree.hpp
        serialization.hpp
        json_conversion.hpp
)

target_include_directories(
        zktrade

        PUBLIC
        .
        ${DEPENDS_DIR}/libsnark
        ${DEPENDS_DIR}/libsnark/depends/libff
        ${DEPENDS_DIR}/libsnark/depends/libfqfft
)

include_directories(.)

add_executable(
        setup

        setup/run_setup.cpp
        MerkleTree.cpp
        serialization.cpp
)
target_link_libraries(
        setup

        snark
)
target_include_directories(
        setup

        PUBLIC
        ${DEPENDS_DIR}/libsnark
        ${DEPENDS_DIR}/libsnark/depends/libff
        ${DEPENDS_DIR}/libsnark/depends/libfqfft
)
add_executable(
        server

        server/run_server.cpp
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
