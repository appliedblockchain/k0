#include <libff/common/utils.hpp>

class MerkleTree {
  public:
    MerkleTree(size_t depth);
    libff::bit_vector root();
    libff::bit_vector operator[](uint i);
    uint add(libff::bit_vector leaf);
    std::tuple<uint, libff::bit_vector, std::vector<libff::bit_vector>> simulate_add(libff::bit_vector leaf);
    uint num_elements();
    std::vector<libff::bit_vector> path(uint address);
    void print();
  private:
    std::vector<libff::bit_vector>* node_levels;
    libff::bit_vector* empty_tree_roots;
    std::vector<libff::bit_vector>* leaves;
    size_t depth;
    uint max_size;
    libff::bit_vector internal_node_at(uint level, uint position);
};