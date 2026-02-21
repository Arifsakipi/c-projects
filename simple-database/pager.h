#pragma once
#include "defs.h"

void    *get_page(Pager *pager, uint32_t page_num);
uint32_t get_unused_page_num(Pager *pager);

Pager *pager_open(const char *filename);
void   pager_flush(Pager *pager, uint32_t page_num);
Table *db_open(const char *filename);
void   db_close(Table *table);
