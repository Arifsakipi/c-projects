#pragma once
#include "defs.h"

/* Input buffer */
InputBuffer *new_input_buffer(void);
void         read_input(InputBuffer *input_buffer);
void         close_input_buffer(InputBuffer *input_buffer);
void         print_prompt(void);

/* Tree / constant printing */
void print_constants(void);
void indent(uint32_t level);
void print_tree(Pager *pager, uint32_t page_num, uint32_t indentation_level);

/* REPL */
PrepareResult     prepare_statement(InputBuffer *input_buffer,
                                    Statement *statement);
ExecuteResult     execute_statement(Statement *statement, Table *table);
MetaCommandResult do_meta_command(InputBuffer *input_buffer, Table *table);
