# include "IntHashTable.h"

static inline int _IntHashTable_hash(int key) {
	return ((unsigned int)key) % INT_HASH_TABLE_SIZE;
}

void	IntHashTable_init(IntHashTable *t) {
	for (int i = 0; i < INT_HASH_TABLE_SIZE; i++) {
		List_init(&t->table[i]);
	}
}

void	IntHashTable_clear(IntHashTable *t) {
	for (int i = 0; i < INT_HASH_TABLE_SIZE; i++) {
		ListHead *l = &t->table[i];

		if (l->size && l->first) {
			ListItem* aux = l->first;
			while (aux) {
				IntHashItem *item = (IntHashItem *) aux;
				assert(item && "IntHashTable_clear | invalid token cast");
				aux = aux->next;
				free(item);
			}
			List_init(l);
		}
	}
}

void	IntHashTable_add(IntHashTable *t, int key, int value) {
	int *test = IntHashTable_get(t, key);
	if (test) {
		*test = value;
		return ;
	}

	int hash_key = _IntHashTable_hash(key);
	IntHashItem *item = (IntHashItem *) malloc(sizeof(IntHashItem));
	if (! item) handle_error("IntHashTable_add | malloc error");

	item->list.next = item->list.prev = 0;
	item->value = value;
	item->key = key;
	ListHead *list = &t->table[hash_key];
	List_insert(list, list->last, &item->list);
}

int		*IntHashTable_get(IntHashTable *t, int key) {
	int hash_key = _IntHashTable_hash(key);

	for (ListItem *it = t->table[hash_key].first; it; it = it->next) {
		IntHashItem *item = (IntHashItem *) it;
		assert(item && "IntHashTable_get | invalid cast");
		if ((item->key == key))
			return (&item->value);
	}
	return (0);
}

void	IntHashTable_remove(IntHashTable *t, int key) {
	int hash_key = _IntHashTable_hash(key);
	ListHead *list = &t->table[hash_key];

	for (ListItem *it = list->first; it; it = it->next) {
		IntHashItem *item = (IntHashItem *) it;
		assert(item && "IntHashTable_get | invalid cast");
		if ((item->key == key)) {
			List_detach(list, it);
			free(item);
			return ;
		}
	}
}
