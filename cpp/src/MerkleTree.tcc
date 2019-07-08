#ifndef K0_MERKLETREE_TCC
#define K0_MERKLETREE_TCC

#include <bitset>
#include <iomanip>
#include <iostream>
#include <vector>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include "MerkleTree.hpp"
#include "serialization.hpp"
#include "util.h"

using namespace std;
using namespace libff;
using namespace libsnark;

template<typename HashT>
bit_vector hashLR(bit_vector left, bit_vector right) {
  bit_vector block = left;
  block.insert(block.end(), right.begin(), right.end());
  return HashT::get_hash(block);
}

template<typename HashT>
MerkleTree<HashT>::MerkleTree(size_t d) :
  depth{d}
{
  cout << "MT CONSTRUCTOR" << endl;
  max_size = exp2(depth);
  empty_tree_roots = new bit_vector[depth];
  node_levels = new vector<bit_vector>[depth];
  leaves = new vector<bit_vector>();
  auto all_zeros = hex2bits("0x0000000000000000000000000000000000000000000000000000000000000000");
  for (uint i = 0; i < depth; i++) {
    bit_vector prev;
    if (i == 0) {
      prev = all_zeros;
    } else {
      prev = empty_tree_roots[i - 1];
    }
    empty_tree_roots[i] = hashLR<HashT>(prev, prev);
  }
  node_levels[depth - 1].push_back(empty_tree_roots[depth - 1]);
}

template<typename HashT>
bit_vector MerkleTree<HashT>::root() {
  return node_levels[depth - 1][0];
}

template<typename HashT>
uint MerkleTree<HashT>::num_elements() {
  return leaves->size();
}

template<typename HashT>
bit_vector MerkleTree<HashT>::operator[](uint i) {
  return leaves->at(i);
}

template<typename HashT>
uint MerkleTree<HashT>::add(bit_vector leaf) {
  const uint address = leaves->size();
  if (leaves->size() >= max_size) {
    throw overflow_error("Tree is full");
  }
  leaves->push_back(leaf);
  uint prev_level_pos = address;

  auto all_zeros = hex2bits("0x0000000000000000000000000000000000000000000000000000000000000000");
  for (uint level = 0; level < depth; level++) {
    // First we need to find out which element at the current level gets updated
    // (position at previous level divided by 2).
    const uint pos = prev_level_pos / 2;
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
    bit_vector new_hash = hashLR<HashT>(left, right);
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

template<typename HashT>
vector<bit_vector> MerkleTree<HashT>::path(uint address) {
  vector<bit_vector> p;
  uint prev_pos;
  for (uint row = 0; row < depth; row++) {
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
      uint pos = prev_pos / 2;
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

template<typename HashT>
tuple<uint, bit_vector, vector<bit_vector>> MerkleTree<HashT>::simulate_add(bit_vector leaf) {
  uint address = leaves->size();
  vector<bit_vector> p;
  uint prev_pos;
  bit_vector prev_hash;
  for (uint row = 0; row < depth; row++) {
    bit_vector sibling;
    bit_vector hash;
    if (row == 0) {
      if (address % 2) {
        // right side
        sibling = internal_node_at(0, address - 1);
        hash = hashLR<HashT>(sibling, leaf);
      } else {
        sibling = internal_node_at(0, address + 1);
        hash = hashLR<HashT>(leaf, sibling);
      }
      prev_pos = address;
    } else {
      uint pos = prev_pos / 2;
      if (pos % 2) {
        // right side
        sibling = internal_node_at(row, pos - 1);
        hash = hashLR<HashT>(sibling, prev_hash);
      } else {
        sibling = internal_node_at(row, pos + 1);
        hash = hashLR<HashT>(prev_hash, sibling);
      }
      prev_pos = pos;
    }
    p.insert(p.begin(), sibling);
    prev_hash = hash;
  }
  // prev_hash will now be the root
  return make_tuple(address, prev_hash, p);
}

template<typename HashT>
bit_vector MerkleTree<HashT>::internal_node_at(uint row, uint pos) {
  auto all_zeros = hex2bits("0x0000000000000000000000000000000000000000000000000000000000000000");
  if (row == 0) {
    if (pos + 1 <= leaves->size()) {
      return leaves->at(pos);
    } else {
      return all_zeros;
    }
  } else {
    // row 0 is the leaves, row 1 is (node_level) level 0 etc.
    uint level = row - 1;
    if (pos + 1 <= node_levels[level].size()) {
      return node_levels[level][pos];
    } else {
      return empty_tree_roots[level];
    }
  }
}

template<typename HashT>
void MerkleTree<HashT>::print() {
  auto all_zeros = hex2bits("0x0000000000000000000000000000000000000000000000000000000000000000");
  for (int level = depth - 1; level >= 0; level--) {
    uint num_elems = exp2(depth - level - 1);
    for (uint pos = 0; pos < num_elems; pos++) {
      if (node_levels[level].size() - 1 >= pos) {
        k0::printnode(node_levels[level][pos]);
      } else {
        k0::printnode(empty_tree_roots[level]);
      }
    }
    cout << endl;
  }
  for (uint pos = 0; pos < max_size; pos++) {
    if (leaves->size() - 1 >= pos) {
      k0::printnode(leaves->at(pos));
    } else {
      k0::printnode(all_zeros);
    }
  }
  cout << endl;
}

#endif // K0_MERKLETREE_TCC
