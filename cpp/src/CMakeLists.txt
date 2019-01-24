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

add_subdirectory(examples)