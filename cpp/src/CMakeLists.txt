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

add_subdirectory(examples)
include_directories(.)

add_library(zktrade "")

target_sources(
        zktrade

        PRIVATE
        crypto/sha256.cpp
        hash/sha256_compression.cpp
        scheme/cm.cpp
        scheme/prfs.cpp
        util.cpp

        PUBLIC
        circuitry/gadgets/prfs.h
        hash/sha256_compression.h
        scheme/cm.h
        scheme/prfs.h
        util.h
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