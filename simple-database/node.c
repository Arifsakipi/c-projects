#include "node.h"

// Common node accessors

NodeType get_node_type(void *node) {
  uint8_t value = *((uint8_t *)((char *)node + NODE_TYPE_OFFSET));
  return (NodeType)value;
}

void set_node_type(void *node, NodeType type) {
  *((uint8_t *)((char *)node + NODE_TYPE_OFFSET)) = (uint8_t)type;
}

bool is_node_root(void *node) {
  uint8_t value = *((uint8_t *)((char *)node + IS_ROOT_OFFSET));
  return (bool)value;
}

void set_node_root(void *node, bool is_root) {
  *((uint8_t *)((char *)node + IS_ROOT_OFFSET)) = (uint8_t)is_root;
}

uint32_t *node_parent(void *node) {
  return (uint32_t *)((char *)node + PARENT_POINTER_OFFSET);
}

// Internal node accessors

uint32_t *internal_node_num_keys(void *node) {
  return (uint32_t *)((char *)node + INTERNAL_NODE_NUM_KEYS_OFFSET);
}

uint32_t *internal_node_right_child(void *node) {
  return (uint32_t *)((char *)node + INTERNAL_NODE_RIGHT_CHILD_OFFSET);
}

uint32_t *internal_node_cell(void *node, uint32_t cell_num) {
  return (uint32_t *)((char *)node + INTERNAL_NODE_HEADER_SIZE +
                      cell_num * INTERNAL_NODE_CELL_SIZE);
}

uint32_t *internal_node_child(void *node, uint32_t child_num) {
  uint32_t num_keys = *internal_node_num_keys(node);
  if (child_num > num_keys) {
    printf("Tried to access child_num %d > num_keys %d\n", child_num, num_keys);
    exit(EXIT_FAILURE);
  }
  if (child_num == num_keys) {
    uint32_t *right_child = internal_node_right_child(node);
    if (*right_child == INVALID_PAGE_NUM) {
      printf("Tried to access right child of node, but was invalid page\n");
      exit(EXIT_FAILURE);
    }
    return right_child;
  }
  uint32_t *child = internal_node_cell(node, child_num);
  if (*child == INVALID_PAGE_NUM) {
    printf("Tried to access child %d of node, but was invalid page\n",
           child_num);
    exit(EXIT_FAILURE);
  }
  return child;
}

uint32_t *internal_node_key(void *node, uint32_t key_num) {
  return (uint32_t *)((char *)internal_node_cell(node, key_num) +
                      INTERNAL_NODE_CHILD_SIZE);
}

// Leaf node accessors

uint32_t *leaf_node_num_cells(void *node) {
  return (uint32_t *)((char *)node + LEAF_NODE_NUM_CELLS_OFFSET);
}

uint32_t *leaf_node_next_leaf(void *node) {
  return (uint32_t *)((char *)node + LEAF_NODE_NEXT_LEAF_OFFSET);
}

void *leaf_node_cell(void *node, uint32_t cell_num) {
  return (char *)node + LEAF_NODE_HEADER_SIZE + cell_num * LEAF_NODE_CELL_SIZE;
}

uint32_t *leaf_node_key(void *node, uint32_t cell_num) {
  return (uint32_t *)leaf_node_cell(node, cell_num);
}

void *leaf_node_value(void *node, uint32_t cell_num) {
  return (char *)leaf_node_cell(node, cell_num) + LEAF_NODE_KEY_SIZE;
}

// Initialization

void initialize_leaf_node(void *node) {
  set_node_type(node, NODE_LEAF);
  set_node_root(node, false);
  *leaf_node_num_cells(node) = 0;
  *leaf_node_next_leaf(node) = 0; /* 0 == no sibling */
}

void initialize_internal_node(void *node) {
  set_node_type(node, NODE_INTERNAL);
  set_node_root(node, false);
  *internal_node_num_keys(node)    = 0;
  *internal_node_right_child(node) = INVALID_PAGE_NUM;
}

// Max key in a node (used during splits)

uint32_t get_node_max_key(void *node) {
  switch (get_node_type(node)) {
  case NODE_INTERNAL:
    return *internal_node_key(node, *internal_node_num_keys(node) - 1);
  case NODE_LEAF:
    return *leaf_node_key(node, *leaf_node_num_cells(node) - 1);
  }
  exit(EXIT_FAILURE); /* unreachable */
}

// Binary search for a child index inside an internal node (Part 11)

uint32_t internal_node_find_child(void *node, uint32_t key) {
  uint32_t num_keys  = *internal_node_num_keys(node);
  uint32_t min_index = 0;
  uint32_t max_index = num_keys; /* one more child than keys */

  while (min_index != max_index) {
    uint32_t index        = (min_index + max_index) / 2;
    uint32_t key_to_right = *internal_node_key(node, index);
    if (key_to_right >= key) {
      max_index = index;
    } else {
      min_index = index + 1;
    }
  }
  return min_index;
}
