#include <libff/common/utils.hpp>

class MerkleTree {
  public:
    MerkleTree(size_t depth);
    libff::bit_vector root();
    libff::bit_vector operator[](size_t i);
    size_t add(libff::bit_vector leaf);
    std::tuple<size_t, libff::bit_vector, std::vector<libff::bit_vector>> simulate_add(libff::bit_vector leaf);
    size_t num_elements();
    std::vector<libff::bit_vector> path(size_t address);
    void print();
  private:
    std::vector<libff::bit_vector>* node_levels;
    libff::bit_vector* empty_tree_roots;
    std::vector<libff::bit_vector>* leaves;
    size_t depth;
    size_t max_size;
    libff::bit_vector internal_node_at(size_t level, size_t position);
};