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
  circuit_test

  circuit_test.cpp
  serialization.cpp
  MerkleTree.cpp
  ../depends/zcash/src/zcash/Address.cpp
  ../depends/zcash/src/zcash/prf.cpp
  ../depends/zcash/src/zcash/NoteEncryption.cpp
  ../depends/zcash/src/zcash/Note.cpp
  ../depends/zcash/src/zcash/util.cpp
  ../depends/zcash/src/support/cleanse.cpp
  ../depends/zcash/src/crypto/sha256.cpp
  ../depends/zcash/src/uint256.cpp
  ../depends/zcash/src/utilstrencodings.cpp
)
target_link_libraries(
  circuit_test

  snark
  sodium
)
target_include_directories(
  circuit_test

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
        util.cpp
        crypto/sha256.cpp
        hash/sha256_compression.cpp
        prfs.cpp
        PUBLIC
        prfs.h
        util.h
        circuitry/gadgets/prfs.h
        hash/sha256_compression.h
)

target_include_directories(
        zktrade

        PUBLIC
        .
        ${DEPENDS_DIR}/libsnark
        ${DEPENDS_DIR}/libsnark/depends/libff
        ${DEPENDS_DIR}/libsnark/depends/libfqfft
)
