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
        hash/sha256.cpp
        hash/sha256_compression.cpp
        scheme/comms.tcc
        scheme/ka.cpp
        scheme/kdf.cpp
        scheme/note_encryption.cpp
        scheme/prfs.tcc
        util.cpp
        serialization.cpp
        json_conversion.cpp
        server/proof_serialization.cpp

        PUBLIC
        hash/sha256.hpp
        hash/sha256_compression.hpp
        scheme/comms.hpp
        scheme/ka.hpp
        scheme/kdf.hpp
        scheme/prfs.h
        server/Server.hpp
        server/Server.tcc
        util.h
        MerkleTree.hpp
        MerkleTree.tcc
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
        /usr/local/include
)

include_directories(.)

add_executable(
        setup

        setup/run_setup.cpp
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
        json_conversion.cpp
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
        hash

        hash.cpp
)
target_link_libraries(
        hash

        snark
        zktrade
)
target_include_directories(
        hash

        PUBLIC
        ${DEPENDS_DIR}/libsnark
        ${DEPENDS_DIR}/libsnark/depends/libff
        ${DEPENDS_DIR}/libsnark/depends/libfqfft
)

add_executable(
        unpack_256_bits

        unpack_256_bits.cpp
)
target_link_libraries(
        unpack_256_bits

        snark
        zktrade
)
target_include_directories(
        unpack_256_bits

        PUBLIC
        ${DEPENDS_DIR}/libsnark
        ${DEPENDS_DIR}/libsnark/depends/libff
        ${DEPENDS_DIR}/libsnark/depends/libfqfft
)

add_executable(
        mtserver

        mtserver/run_mtserver.cpp
)
target_link_libraries(
        mtserver

        snark
        zktrade
        jsoncpp
        microhttpd
        jsonrpccpp-common
        jsonrpccpp-server
)
target_include_directories(
        mtserver

        PUBLIC
        ${DEPENDS_DIR}/libsnark
        ${DEPENDS_DIR}/libsnark/depends/libff
        ${DEPENDS_DIR}/libsnark/depends/libfqfft
)

add_executable(
        iotest

        iotest.cpp
)
target_link_libraries(
        iotest

        snark
        zktrade
)
target_include_directories(
        iotest

        PUBLIC
        ${DEPENDS_DIR}/libsnark
        ${DEPENDS_DIR}/libsnark/depends/libff
        ${DEPENDS_DIR}/libsnark/depends/libfqfft
)
