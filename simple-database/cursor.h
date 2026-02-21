#pragma once
#include "defs.h"

Cursor *leaf_node_find(Table *table, uint32_t page_num, uint32_t key);
Cursor *internal_node_find(Table *table, uint32_t page_num, uint32_t key);
Cursor *table_find(Table *table, uint32_t key);
Cursor *table_start(Table *table);
void   *cursor_value(Cursor *cursor);
void    cursor_advance(Cursor *cursor);
