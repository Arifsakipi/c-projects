#include "repl.h"
#include "btree.h"
#include "cursor.h"
#include "node.h"
#include "pager.h"
#include "row.h"
#include <string.h>

// Input buffer

InputBuffer *new_input_buffer(void) {
  InputBuffer *input_buffer = malloc(sizeof(InputBuffer));
  input_buffer->buffer        = NULL;
  input_buffer->buffer_length = 0;
  input_buffer->input_length  = 0;
  return input_buffer;
}

void print_prompt(void) { printf("db > "); }

void read_input(InputBuffer *input_buffer) {
  ssize_t bytes_read =
      getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);
  if (bytes_read <= 0) {
    printf("Error reading input\n");
    exit(EXIT_FAILURE);
  }
  input_buffer->input_length              = bytes_read - 1;
  input_buffer->buffer[bytes_read - 1]    = '\0';
}

void close_input_buffer(InputBuffer *input_buffer) {
  free(input_buffer->buffer);
  free(input_buffer);
}

// Print helpers

void print_constants(void) {
  printf("ROW_SIZE: %u\n",                  ROW_SIZE);
  printf("COMMON_NODE_HEADER_SIZE: %u\n",   COMMON_NODE_HEADER_SIZE);
  printf("LEAF_NODE_HEADER_SIZE: %u\n",     LEAF_NODE_HEADER_SIZE);
  printf("LEAF_NODE_CELL_SIZE: %u\n",       LEAF_NODE_CELL_SIZE);
  printf("LEAF_NODE_SPACE_FOR_CELLS: %u\n", LEAF_NODE_SPACE_FOR_CELLS);
  printf("LEAF_NODE_MAX_CELLS: %u\n",       LEAF_NODE_MAX_CELLS);
  printf("INTERNAL_NODE_MAX_CELLS: %u\n",   INTERNAL_NODE_MAX_CELLS);
}

void indent(uint32_t level) {
  for (uint32_t i = 0; i < level; i++) printf("  ");
}

/* Part 12: recursively print the whole B-tree */
void print_tree(Pager *pager, uint32_t page_num, uint32_t indentation_level) {
  void    *node = get_page(pager, page_num);
  uint32_t num_keys, child;

  switch (get_node_type(node)) {
  case NODE_LEAF:
    num_keys = *leaf_node_num_cells(node);
    indent(indentation_level);
    printf("- leaf (size %d)\n", num_keys);
    for (uint32_t i = 0; i < num_keys; i++) {
      indent(indentation_level + 1);
      printf("- %d\n", *leaf_node_key(node, i));
    }
    break;

  case NODE_INTERNAL:
    num_keys = *internal_node_num_keys(node);
    indent(indentation_level);
    printf("- internal (size %d)\n", num_keys);
    for (uint32_t i = 0; i < num_keys; i++) {
      child = *internal_node_child(node, i);
      print_tree(pager, child, indentation_level + 1);
      indent(indentation_level + 1);
      printf("- key %d\n", *internal_node_key(node, i));
    }
    child = *internal_node_right_child(node);
    print_tree(pager, child, indentation_level + 1);
    break;
  }
}

// Statement preparation

PrepareResult prepare_statement(InputBuffer *input_buffer,
                                Statement   *statement) {
  if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
    statement->type = STATEMENT_INSERT;
    int args_assigned =
        sscanf(input_buffer->buffer, "insert %u %31s %254s",
               &(statement->row_to_insert.id),
               statement->row_to_insert.username,
               statement->row_to_insert.email);
    if (args_assigned < 3) return PREPARE_SYNTAX_ERROR;
    return PREPARE_SUCCESS;
  }
  if (strcmp(input_buffer->buffer, "select") == 0) {
    statement->type = STATEMENT_SELECT;
    return PREPARE_SUCCESS;
  }
  return PREPARE_UNRECOGNIZED_STATEMENT;
}

// Statement execution

static ExecuteResult execute_insert(Statement *statement, Table *table) {
  Row     *row_to_insert = &(statement->row_to_insert);
  uint32_t key_to_insert  = row_to_insert->id;

  Cursor *cursor = table_find(table, key_to_insert);

  void    *node      = get_page(table->pager, cursor->page_num);
  uint32_t num_cells = *leaf_node_num_cells(node);

  /* Part 9: reject duplicate keys */
  if (cursor->cell_num < num_cells) {
    uint32_t key_at_index = *leaf_node_key(node, cursor->cell_num);
    if (key_at_index == key_to_insert) {
      free(cursor);
      return EXECUTE_DUPLICATE_KEY;
    }
  }

  leaf_node_insert(cursor, row_to_insert->id, row_to_insert);
  free(cursor);
  return EXECUTE_SUCCESS;
}

static ExecuteResult execute_select(Statement *statement __attribute__((unused)),
                                    Table     *table) {
  Cursor *cursor = table_start(table);
  Row     row;

  while (!cursor->end_of_table) {
    deserialize_row(cursor_value(cursor), &row);
    print_row(&row);
    cursor_advance(cursor);
  }

  free(cursor);
  return EXECUTE_SUCCESS;
}

ExecuteResult execute_statement(Statement *statement, Table *table) {
  switch (statement->type) {
  case STATEMENT_INSERT: return execute_insert(statement, table);
  case STATEMENT_SELECT: return execute_select(statement, table);
  }
  return EXECUTE_SUCCESS;
}

// Meta commands

MetaCommandResult do_meta_command(InputBuffer *input_buffer, Table *table) {
  if (strcmp(input_buffer->buffer, ".exit") == 0) {
    close_input_buffer(input_buffer);
    db_close(table);
    exit(EXIT_SUCCESS);
  }
  if (strcmp(input_buffer->buffer, ".btree") == 0) {
    printf("Tree:\n");
    print_tree(table->pager, 0, 0);
    return META_COMMAND_SUCCESS;
  }
  if (strcmp(input_buffer->buffer, ".constants") == 0) {
    printf("Constants:\n");
    print_constants();
    return META_COMMAND_SUCCESS;
  }
  return META_COMMAND_UNRECOGNIZED_COMMAND;
}
