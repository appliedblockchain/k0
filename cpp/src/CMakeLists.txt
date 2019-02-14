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

add_executable(
        playground

        playground.cpp
        crypto/sha256.cpp
)
target_link_libraries(
        playground

        snark
)
target_include_directories(
        playground

        PUBLIC
        ${DEPENDS_DIR}/libsnark
        ${DEPENDS_DIR}/libsnark/depends/libff
        ${DEPENDS_DIR}/libsnark/depends/libfqfft
)

add_library(zktrade "")

target_sources(
        zktrade

        PRIVATE
        crypto/sha256.cpp
        hash/sha256_compression.cpp
        scheme/cm.cpp
        scheme/prfs.cpp
        util.cpp
        MerkleTree.cpp
        serialization.cpp
        json_conversion.cpp

        PUBLIC
        circuitry/gadgets/prfs.h
        hash/sha256_compression.h
        scheme/cm.h
        scheme/prfs.h
        server/MerkleTreeServer.hpp
        server/MerkleTreeServer.tcc
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

add_subdirectory(setup)

add_subdirectory(server)

include_directories(.)