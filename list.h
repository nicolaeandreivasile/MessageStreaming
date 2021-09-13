#ifndef _LIST_H
#define _LIST_H 1

/* list structure */
typedef struct list {

  void* info;
  struct list* next;
} list;

#define INFO(L)		((list*) L)->info
#define NEXT(L)		((list*) L)->next

/* list functions */
list* make_cell(void* x, size_t dim);
void free_cell(list** l);
int add_cell(list** l, void* x, size_t dim, int (*cmp)(void*, void*));
int add_last(list** l, void* x, size_t dim);
int remove_cell(list** l, void* x, int (*cmp)(void*, void*));
int remove_first(list** l);
int contain_cell(list* l, void* x, int (*cmp)(void*, void*));
list* get_cell(list* l, void* x, int (*cmp)(void*, void*));

#endif
