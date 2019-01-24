add_executable(merkle_tree_nursery nursery.cpp ../../MerkleTree.cpp)
target_link_libraries(merkle_tree_nursery snark)
target_include_directories(
  merkle_tree_nursery

  PUBLIC
  ${DEPENDS_DIR}/libsnark
  ${DEPENDS_DIR}/libsnark/depends/libff
  ${DEPENDS_DIR}/libsnark/depends/libfqfft
)

add_executable(
  merkle_tree_server

  server.cpp
  ../../MerkleTree.cpp
  ../../serialization.cpp
  ../../json_conversion.cpp
)
target_link_libraries(
  merkle_tree_server

  snark
  jsoncpp
  microhttpd
  jsonrpccpp-common
  jsonrpccpp-server
)
target_include_directories(
  merkle_tree_server

  PUBLIC
  ${DEPENDS_DIR}/libsnark
  ${DEPENDS_DIR}/libsnark/depends/libff
  ${DEPENDS_DIR}/libsnark/depends/libfqfft
)

add_executable(
  merkle_tree_addition_test

  addition_test.cpp
  ../../MerkleTree.cpp
  ../../serialization.cpp
)
target_link_libraries(
  merkle_tree_addition_test

  snark
)
target_include_directories(
  merkle_tree_addition_test

  PUBLIC
  ${DEPENDS_DIR}/libsnark
  ${DEPENDS_DIR}/libsnark/depends/libff
  ${DEPENDS_DIR}/libsnark/depends/libfqfft
)

add_executable(
  merkle_tree_addition_packed_test

  addition_packed_test.cpp
  ../../MerkleTree.cpp
  ../../serialization.cpp
)
target_link_libraries(
  merkle_tree_addition_packed_test

  snark
)
target_include_directories(
  merkle_tree_addition_packed_test

  PUBLIC
  ${DEPENDS_DIR}/libsnark
  ${DEPENDS_DIR}/libsnark/depends/libff
  ${DEPENDS_DIR}/libsnark/depends/libfqfft
)

add_executable(
  merkle_tree_addition_circuit_test

  addition_circuit_test.cpp
  ../../MerkleTree.cpp
  ../../serialization.cpp
)
target_link_libraries(
  merkle_tree_addition_circuit_test

  snark
)
target_include_directories(
  merkle_tree_addition_circuit_test

  PUBLIC
  ${DEPENDS_DIR}/libsnark
  ${DEPENDS_DIR}/libsnark/depends/libff
  ${DEPENDS_DIR}/libsnark/depends/libfqfft
)

add_executable(
  merkle_tree_addition_setup

  addition_setup.cpp
  ../../MerkleTree.cpp
  ../../serialization.cpp
)
target_link_libraries(
  merkle_tree_addition_setup

  snark
)
target_include_directories(
  merkle_tree_addition_setup

  PUBLIC
  ${DEPENDS_DIR}/libsnark
  ${DEPENDS_DIR}/libsnark/depends/libff
  ${DEPENDS_DIR}/libsnark/depends/libfqfft
)

add_executable(
  merkle_tree_addition_verify

  addition_verify.cpp
)
target_link_libraries(
  merkle_tree_addition_verify

  snark
)
target_include_directories(
  merkle_tree_addition_verify

  PUBLIC
  ${DEPENDS_DIR}/libsnark
  ${DEPENDS_DIR}/libsnark/depends/libff
  ${DEPENDS_DIR}/libsnark/depends/libfqfft
)

add_executable(
  merkle_tree_inclusion_test

  inclusion_test.cpp
  ../../MerkleTree.cpp
  ../../serialization.cpp
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


add_executable(
  merkle_tree_inclusion_setup

  inclusion_setup.cpp
)
target_link_libraries(
  merkle_tree_inclusion_setup

  snark
)
target_include_directories(
  merkle_tree_inclusion_setup

  PUBLIC
  ${DEPENDS_DIR}/libsnark
  ${DEPENDS_DIR}/libsnark/depends/libff
  ${DEPENDS_DIR}/libsnark/depends/libfqfft
)

