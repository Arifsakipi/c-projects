#pragma once
#include "defs.h"

/* Common node accessors */
NodeType  get_node_type(void *node);
void      set_node_type(void *node, NodeType type);
bool      is_node_root(void *node);
void      set_node_root(void *node, bool is_root);
uint32_t *node_parent(void *node);

/* Internal node accessors */
uint32_t *internal_node_num_keys(void *node);
uint32_t *internal_node_right_child(void *node);
uint32_t *internal_node_cell(void *node, uint32_t cell_num);
uint32_t *internal_node_child(void *node, uint32_t child_num);
uint32_t *internal_node_key(void *node, uint32_t key_num);

/* Leaf node accessors */
uint32_t *leaf_node_num_cells(void *node);
uint32_t *leaf_node_next_leaf(void *node);
void     *leaf_node_cell(void *node, uint32_t cell_num);
uint32_t *leaf_node_key(void *node, uint32_t cell_num);
void     *leaf_node_value(void *node, uint32_t cell_num);

/* Initialization */
void initialize_leaf_node(void *node);
void initialize_internal_node(void *node);

/* Max key in a node — used during splits */
uint32_t get_node_max_key(void *node);

/* Binary search for a child index inside an internal node */
uint32_t internal_node_find_child(void *node, uint32_t key);
