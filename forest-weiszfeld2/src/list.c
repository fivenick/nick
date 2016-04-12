/*
 *author hepan
 * 实现list的基本功能，基本数据机构声明在list.h里
 * 
 */

#include "../include/list.h"
#include "../include/log.h"

int defaultfree(void *data)
{
    free(data);
}
/*
 * function : 创建一个默认的list
 * input    : none
 * output   : 没有指定free函数的默认list
 */
struct list *list_create()
{
	struct list *list = my_malloc_type(struct list);
	list->size = 0;
	list->head = NULL;
    list->tail = NULL;
    list->fr = defaultfree;
	return list;
}

/*
 * function : 创建一个默认的list
 * input    : 释放data的回调函数
 * output   : 没有指定free函数的默认list
 */
struct list *list_create_dt(lfr fr)
{
	struct list *list = my_malloc_type(struct list);
	list->size = 0;
	list->head = NULL;
    list->tail = NULL;
    list->fr = fr;
	return list;
}

//从list中删除一个node，并返回这个node包含的数据
int list_delete(struct list *list, int index)
{
	struct list_e * node = NULL;
    if (list->size <= 0 || index >= list->size)
		return -1;
    struct list_e **pos;
    int i = 0;
    for (pos = &(list->head); *pos != NULL;)
    {
        struct list_e *e = *pos;
        if (i == index)
        {
            *pos = e->next;
            lfr fr = list->fr;
            (*fr)(e->data);
            free(e);
            break;
        }
        else
        {
            pos = &(e->next);
            i++;
        }
    }
    list->size--;
    return 1;
    /*
	struct list_e *pos = list->head;
	int i;
	//将指针知道要移除的node的前一个node, 若要删除的是head，则pos也指向第一个
	for (i = 0; i < index - 1 && pos->next != NULL; i++)
		pos = pos->next;
	if (index == 0)
	{//删除head
		node = pos;
		pos = pos->next;
		node->next = NULL;
		list->head = pos;
	}
    else
    {
		node = pos->next;
        if (node != NULL)
    		pos->next = node->next;
        else
            pos->next = NULL;
		node->next = NULL;
	}
    list->size--;
    if (index == list->size - 1)
        list->tail = pos;
	assert(list->size >= 0);
    void *r = node->data;
    free(node);
	return node;
    */
}

//往list尾添加一个node
void list_add(struct list *list,struct list_e *node)
{
	if (list->head  == NULL)
    {
		list->head = node;
        list->tail = node;
    }
    else
    {
        list->tail->next = node;
        list->tail = node;
	}
	list->size++;
}

struct list_e *listnode_create(void *data)
{
	struct list_e *node = my_malloc_type(struct list_e);
	node->next = NULL;
	node->data = data;
	return node;
}

inline size_t list_size(struct list *list)
{
	return list->size;
}


void list_print(struct list* list)
{
	struct list_e* e = list->head;
    int flag = 0;
	while (e)
	{
        flag++;
        if (flag == list->size - 1)
		    printf("%s ", (char *)e->data);
        else
    		printf("%s ", (char *)e->data);
		e = e->next;
	}
    printf("\n");
}


void listnode_destroy(struct list *list, struct list_e *e)
{
    lfr fr = list->fr;
    (*fr)(e->data);
    free(e);
}
//销毁整个list
void list_destroy(struct list *list){
	struct list_e *node;
	while (list->head)
	{
		node = list->head;
		list->head = list->head->next;
        listnode_destroy(list, node);
		list->size--;
	}
   if (list->size != 0)
   {
        printf("list_destroy : list->size != 0 ");
       // log_message(LOG_LEVEL_ERROR, "a");;
   }
   free(list);
};

//得到list某个index的元素,如果越界返回NULL
struct list_e *list_get(struct list *list, int index)
{
	if(index >= list->size || index < 0)
		return NULL;
	struct list_e *node = list->head;
	int i;
	for (i = 0; i < index && node->next != NULL; i++)
		node = node->next;
	return node;
}

void *list_get_value(struct list *list, int index)
{
    struct list_e *e = list_get(list, index);
    if (e == NULL)
        return NULL;
    else
        return e->data;
}

/*
 * function : 获得list头部的元素，并从list中移除
 * input    : list
 * output   : 头部的元素 
 */
void *list_head_out(struct list *list)
{
    if (list->head == NULL)
        return NULL;
    struct list_e *e = list->head;
    list->head = list->head->next;
    list->size--;
    void *result = e->data;
    free(e);
    return result;
}

void list_clear(struct list *list)
{
	while (list->head)
	{   
        struct list_e *temp = list->head;
		list->head = list->head->next;
        if (list->fr != NULL)
            (*(list->fr))(temp->data);
        else
            free(temp->data);
        free(temp);
        list->size-- ;
	}
	assert(list->size == 0);
}

void list_totail(struct list *list,struct list_e *pre, struct list_e *e){
    //e本来就在尾部
    if (e == NULL || e->next == NULL)
        return;
    //e在头部
    if (pre == NULL || list->head == e)
        list->head = e->next;
    else
        pre->next = e->next;
    list->tail->next = e;
    list->tail = e;
    e->next = NULL;
}

void list_toarray(struct list *list,struct list_e **listarray)
{
    struct list_e *e = list->head;
    int i = 0;
    while (e)
    {
        listarray[i] = e;
        e = e->next;
        i++;
    }
}

/* Heap sort the list "l" and reserved only "avalnum" nodes (from head) */
void list_part_heap_sort(struct list *l, int avalnum, fun_hsort_comp comp)
{
	int lsize = list_size(l);
	if (lsize < 2 || avalnum < 0)
		return;
	if (avalnum > lsize)
		avalnum = lsize;
	int i;
	// get all pointers of elements
	void *vpa[lsize];
	struct list_e *nle, *fle;
	nle = l->head;
	for (i = 0; i < lsize; i++)
	{
		vpa[i] = nle->data;	
		nle = nle->next;
	}
	// sort the pointers
	heap_sort(vpa, lsize, avalnum, comp);
	// rebuild the list
	if (avalnum > 0)
	{
		nle = l->head;
		for (i = 0; i < avalnum - 1; i++)
		{
			nle->data = vpa[i];
			nle = nle->next;
		}
		fle = nle->next;
		nle->data = vpa[i];
		nle->next = NULL;
		l->tail = nle;
		l->size = avalnum;
		nle = fle;
		for (i = avalnum; i < lsize ; i++)
		{
			nle->data = vpa[i];
			nle = nle->next;
		}
	}
	else
	{
		fle = l->head;
		l->head = NULL;
		l->tail = NULL;
		l->size = 0;
	}
	// free unuseful nodes
	for (i = avalnum; i < lsize; i++)
	{
		nle = fle;
		fle = fle->next;
        listnode_destroy(l, nle);
	}
}

/* Heap sort the list "l" */
void list_heap_sort(struct list *l, fun_hsort_comp comp)
{
	int lsize = list_size(l);
	if (lsize < 2)
		return;
	int i;
	// get all pointers of elements
	void *vpa[lsize];
	struct list_e *nle = l->head;
	for (i = 0; i < lsize; i++)
	{
		vpa[i] = nle->data;	
		nle = nle->next;
	}
	// sort the pointters
	heap_sort(vpa, lsize, lsize, comp);
	// rebuild the list
	nle = l->head;
	for (i = 0; i < lsize; i++)
	{
		nle->data = vpa[i];
		nle = nle->next;
	}
}

/* Only reserve nodes according to "need[]".
 * 
 * MAKE SURE array "need[]" have elements more than "l->size".
 */
void list_shrink(struct list *l, bool *need)
{
	int lsize = list_size(l);
	int i, head_index, tail_index;
	struct list_e *learray[lsize];
	struct list_e *nle;
	head_index = 0;
	tail_index = lsize - 1;
	nle = l->head;
	for (i = 0; i < lsize; i++)
	{
		if (need[i])
			learray[head_index++] = nle;
		else
			learray[tail_index--] = nle;
		nle = nle->next;
	}
	// rebuild list
	if (head_index > 0)
	{
		nle = learray[0];
		l->head = learray[0];
		l->tail = learray[head_index - 1];
		learray[head_index - 1]->next = NULL;
		l->size = head_index;
	}
	else
	{
		l->head = NULL;
		l->tail = NULL;
		l->size = 0;
	}
	// build relation between useful nodes
	for (i = 0; i < head_index - 1; i++)
		learray[i]->next = learray[i + 1];
	// free unuseful nodes
	for (i = head_index; i < lsize; i++)
		listnode_destroy(l, learray[i]);
}

/*
 * function : 合并两个list
 */
void list_merge(struct list *list1, struct list *list2)
{
    list1->tail->next = list2->head;
    list1->tail = list2->tail;
    list1->size += list2->size;
    free(list2);
}
