add_executable(
  merkle_tree_server

  server.cpp
  ../MerkleTree.cpp
  ../serialization.cpp
  ../json_conversion.cpp
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