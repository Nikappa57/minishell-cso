# pragma once

# include "common.h"

# define INT_HASH_TABLE_SIZE 256

typedef struct IntHashItem {
	ListItem	list;
	int			key;
	int			value;
} IntHashItem;

typedef struct IntHashTable {
	ListHead	table[INT_HASH_TABLE_SIZE];
} IntHashTable;

void	IntHashTable_init(IntHashTable *t);
void	IntHashTable_clear(IntHashTable *t);
void	IntHashTable_add(IntHashTable *t, int key, int value);
int		*IntHashTable_get(IntHashTable *t, int key);
void	IntHashTable_remove(IntHashTable *t, int key);
void	IntHashTable_print(IntHashTable *t);