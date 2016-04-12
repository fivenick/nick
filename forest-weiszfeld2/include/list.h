
#include "common.h"

#ifndef __LIST_H__
#define __LIST_H__ 

/*list的node结构定义*/

typedef int (*lfr)(void *);


struct list_e{
    struct list_e *next;
    void *data;
};

/*list的结构定义*/
struct list{
    struct list_e *head;
    struct list_e *tail;
    lfr fr;   
    size_t size;
};

extern int defaultfree(void *data);

extern struct list *list_create();

extern struct list *list_create_dt(lfr fr);

extern void list_clear(struct list *list);

extern void list_destroy(struct list *list);

extern size_t list_size(struct list *list);

extern void list_add(struct list *list,struct list_e *e);

extern int list_delete(struct list *list, int index);

extern struct list_e *listnode_create(void *data);

extern struct list_e *list_get(struct list *list, int index);

extern void *list_get_value(struct list *list, int index);

extern void list_clear(struct list* list);

extern void list_totail(struct list *list,struct list_e *pre, struct list_e *e);

extern void *list_head_out(struct list *list);

extern void list_toarray(struct list *list,struct list_e **listarray);

extern void listnode_destroy(struct list *list, struct list_e *e);

/* Heap sort the list @l */
extern void
list_heap_sort(struct list *l, fun_hsort_comp comp);

/* Heap sort the list @l and reserved only @availnum nodes (from head) */
extern void
list_part_heap_sort(struct list *l, int availnum, fun_hsort_comp comp);

/* Only reserve nodes according to @need[].
 * 
 * MAKE SURE array @need[] have elements more than @l->size.
 */
extern void 
list_shrink(struct list *l, bool *need);

#endif /* __LIST_H__ */

