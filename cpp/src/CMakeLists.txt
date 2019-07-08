add_library(k0 "")

include_directories(.)

target_sources(
        k0

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

target_link_libraries(
        k0

        sodium
        gmp
        gmpxx
        ff
        crypto
)

if (NOT "${USE_INSTALLED_LIBSNARK}")
target_include_directories(
        k0

        PUBLIC
        .
        ${DEPENDS_DIR}/libsnark
        ${DEPENDS_DIR}/libsnark/depends/libff
        ${DEPENDS_DIR}/libsnark/depends/libfqfft
        /usr/local/include
)
endif ()

add_executable(
        convert_vk

        convert_vk.cpp
)
target_link_libraries(
        convert_vk

        snark
        k0
)

add_executable(
        setup

        setup/run_setup.cpp
        serialization.cpp
)
target_link_libraries(
        setup

        snark
        k0
)

add_executable(
        server

        server/run_server.cpp
        json_conversion.cpp
)
target_link_libraries(
        server

        snark
        k0
        jsoncpp
        microhttpd
        jsonrpccpp-common
        jsonrpccpp-server
)

add_executable(
        mtserver

        mtserver/run_mtserver.cpp
)
target_link_libraries(
        mtserver

        snark
        k0
        jsoncpp
        microhttpd
        jsonrpccpp-common
        jsonrpccpp-server
)
