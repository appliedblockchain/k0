add_executable(
  merkle_tree_addition_setup

  addition_setup.cpp
  ../MerkleTree.cpp
  ../serialization.cpp
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
