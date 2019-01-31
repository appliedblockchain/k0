#include <bitset>
#include <iomanip>
#include <iostream>
#include <vector>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/gadgetlib1/gadgets/hashes/sha256/sha256_gadget.hpp>
#include "MerkleTree.hpp"
#include "serialization.hpp"

using namespace std;
using namespace libff;
using namespace libsnark;

typedef Fr<default_r1cs_ppzksnark_pp> FieldT;
typedef sha256_two_to_one_hash_gadget<FieldT> HashT;

void printnode(vector<bool> bv) {
  uint wordlength = 32;
  bitset<32> word;
  cout << "0x";
  for (uint i = 0; i < bv.size(); i++) {
      word.set(wordlength - 1 - (i % wordlength), bv[i]);
      if (i % wordlength == wordlength - 1 || i == bv.size() - 1) {
        cout << setfill('0') << setw(8) << hex << word.to_ulong();
      }
  }
  cout << " ";
}

bit_vector all_zeros = hex2bits("0x0000000000000000000000000000000000000000000000000000000000000000");

bit_vector hashLR(bit_vector left, bit_vector right) {
  bit_vector block = left;
  block.insert(block.end(), right.begin(), right.end());
  return HashT::get_hash(block);
}

bool returnFalse() {
  return false;
}
MerkleTree::MerkleTree(size_t d) :
  depth{d}
{
  max_size = exp2(depth);
  empty_tree_roots = new bit_vector[depth];
  node_levels = new vector<bit_vector>[depth];
  leaves = new vector<bit_vector>();
  for (size_t i = 0; i < depth; i++) {
    bit_vector prev;
    if (i == 0) {
      prev = all_zeros;
    } else {
      prev = empty_tree_roots[i - 1];
    }
    empty_tree_roots[i] = hashLR(prev, prev);
  }
  node_levels[depth - 1].push_back(empty_tree_roots[depth - 1]);
}

bit_vector MerkleTree::root() {
  return node_levels[depth - 1][0];
}

size_t MerkleTree::num_elements() {
  return leaves->size();
}

bit_vector MerkleTree::operator[](size_t i) {
  return leaves->at(i);
}

size_t MerkleTree::add(bit_vector leaf) {
  const size_t address = leaves->size();
  if (leaves->size() >= max_size) {
    throw overflow_error("Tree is full");
  }
  leaves->push_back(leaf);
  size_t prev_level_pos = address;
  for (size_t level = 0; level < depth; level++) {
    // First we need to find out which element at the current level gets updated
    // (position at previous level divided by 2).
    const size_t pos = prev_level_pos / 2;
    // We store the values of all the nodes, however most of them will only have
    // empty subtrees and therefore have the standard values. We only need to
    // store the actually computed hash values. On each level these will grow
    // from left to right.
    bit_vector left;
    bit_vector right;
    if (level == 0) {
      left = leaves->at(pos * 2);
      if (leaves->size() % 2) {
        // the added node is on the left, the right side is still a null leaf
        right = all_zeros;
      } else {
        right = leaves->at(pos * 2 + 1);
      }
    } else {
      left = node_levels[level-1][pos * 2];
      if (node_levels[level-1].size() % 2) {
        right = empty_tree_roots[level-1];
      } else {
        right = node_levels[level-1][pos * 2 + 1];
      }
    }
    bit_vector new_hash = hashLR(left, right);
    if (node_levels[level].size() - 1 == pos) {
      // need to replace the node
      node_levels[level][pos] = new_hash;
    } else {
      // need to add a new node
      node_levels[level].push_back(new_hash);
    }
    prev_level_pos = pos;
  }
  return address;
}

vector<bit_vector> MerkleTree::path(size_t address) {
  vector<bit_vector> p;
  size_t prev_pos;
  for (size_t row = 0; row < depth; row++) {
    bit_vector sibling;
    if (row == 0) {
      if (address % 2) {
        // right side
        sibling = internal_node_at(0, address - 1);
      } else {
        sibling = internal_node_at(0, address + 1);
      }
      prev_pos = address;
    } else {
      size_t pos = prev_pos / 2;
      if (pos % 2) {
        // right side
        sibling = internal_node_at(row, pos - 1);
      } else {
        sibling = internal_node_at(row, pos + 1);
      }
      prev_pos = pos;
    }
    p.insert(p.begin(), sibling);
  }
  return p;
}

tuple<size_t, bit_vector, vector<bit_vector>> MerkleTree::simulate_add(bit_vector leaf) {
  size_t address = leaves->size();
  vector<bit_vector> p;
  size_t prev_pos;
  bit_vector prev_hash;
  for (size_t row = 0; row < depth; row++) {
    bit_vector sibling;
    bit_vector hash;
    if (row == 0) {
      if (address % 2) {
        // right side
        sibling = internal_node_at(0, address - 1);
        hash = hashLR(sibling, leaf);
      } else {
        sibling = internal_node_at(0, address + 1);
        hash = hashLR(leaf, sibling);
      }
      prev_pos = address;
    } else {
      size_t pos = prev_pos / 2;
      if (pos % 2) {
        // right side
        sibling = internal_node_at(row, pos - 1);
        hash = hashLR(sibling, prev_hash);
      } else {
        sibling = internal_node_at(row, pos + 1);
        hash = hashLR(prev_hash, sibling);
      }
      prev_pos = pos;
    }
    p.insert(p.begin(), sibling);
    prev_hash = hash;
  }
  // prev_hash will now be the root
  return make_tuple(address, prev_hash, p);
}

bit_vector MerkleTree::internal_node_at(size_t row, size_t pos) {
  if (row == 0) {
    if (pos + 1 <= leaves->size()) {
      return leaves->at(pos);
    } else {
      return all_zeros;
    }
  } else {
    // row 0 is the leaves, row 1 is (node_level) level 0 etc.
    size_t level = row - 1;
    if (pos + 1 <= node_levels[level].size()) {
      return node_levels[level][pos];
    } else {
      return empty_tree_roots[level];
    }
  }
}

void MerkleTree::print() {
  for (int level = depth - 1; level >= 0; level--) {
    size_t num_elems = exp2(depth - level - 1);
    for (size_t pos = 0; pos < num_elems; pos++) {
      if (node_levels[level].size() - 1 >= pos) {
        printnode(node_levels[level][pos]);
      } else {
        printnode(empty_tree_roots[level]);
      }
    }
    cout << endl;
  }
  for (size_t pos = 0; pos < max_size; pos++) {
    if (leaves->size() - 1 >= pos) {
      printnode(leaves->at(pos));
    } else {
      printnode(all_zeros);
    }
  }
  cout << endl;
}
