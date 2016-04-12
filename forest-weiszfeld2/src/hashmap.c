/*
 * =====================================================================================
 *
 *       Filename:  hashmap.c
 *
 *    Description:  hashmap结构
 *
 *        Version:  1.0
 *        Created:  2012年07月16日 11时15分02秒
 *       Revision:  1.0
 *       Compiler:  gcc
 *
 *         Author:  hepan
 *   Organization:  tigerknows
 *
 * =====================================================================================
 */

#include "../include/hashmap.h"
#include "../include/list.h"

int freeo(void *t)
{
    free(t);
    return 1;
}
int string_compa(void *str1, void *str2)
{
    return strcmp((char *)str1, (char *)str2);
}

int int_compa(void *str1, void *str2)
{
    int *p = (int *)str1;
    int *q = (int *)str2;
    if (*p == *q)
        return 0;
    if (*p > *q)
        return 1;
    if (*p < *q)
        return -1;
}
//获取hash值
static long hash_string(const char *key)
{
    int len = strlen(key);
	long h = 0;
	int i,off = 0;
	for (i = 0; i < len; i++)
		h = 31 * h + key[off++];/* (h<<5)-h+key[i]; */
	if (h < 0) 
		h *= -1;
	return h;
}

//获取hash值
static long hash_int(const int *key)
{
	return (long) (*key);
}

static long hash_calc(const char keytype, void *key)
{
    if (keytype == STRING_KEY)
        return hash_string((char *)key);
    if (keytype == INT_KEY)
        return hash_int((int *)key);   
} 

//释放每个key下的element
static int hmap_key_destroy(struct hmap_e *e, hfr fr)
{
	struct hmap_e *pos = e;
	struct hmap_e *next = pos;
	int count = 0;//计数器：返回free的元素个数
	while (pos)
	{	
		next = pos->next;
		free(pos->key);
        (*fr)(pos->value);
		free(pos);
		count++;
		pos = next;
	}
	return count;
}

int hmap_calcsize(struct hmap *mp)
{
    int res = 0;
    int i;
    for (i = 0; i < mp->hsize; i++)
    {
        struct hmap_e *e = mp->em[i];
        while(e)
        {
            res ++;
            e = e->next;
        }
    }
    return res;

}


/* 
 * function : 获得一个hmap的元素，hashmap内部使用
 * input    : hashmap, key
 * output   : hashmap元素
 */
struct hmap_e *hmap_get_e(struct hmap *mp, void *key)
{
	if(key == NULL)
        return NULL;
	long h = hash_calc(mp->keytype, key) % (mp->hsize);
	struct hmap_e *e = mp->em[h];
	while (e)
	{
		if((*(mp->cpa))(key, e->key) == 0)
			return e;
        e = e->next;
	}
	return NULL;
}


/*
 * function : 创建一个hmap的元素
 * input    : 放入map的key，value，和释放free空间回调函数
 * output   : 创建好的hmap元素
 */
struct hmap_e *hmap_e_create(void *key, void *value)
{
	struct hmap_e *e = my_malloc_type(struct hmap_e);
	e->key = key;
	e->value = value;
	e->next = NULL;
	return e;
}

/* 
 * function : 创建默认hmap，hash槽1024个
 * input    : none
 * output   : 创建好的空hashmap
 */
struct hmap *hmap_create()
{
	struct hmap *mp = my_malloc_type(struct hmap);
	mp->size = 0;
    mp->hsize = KEY_SIZE;
    mp->fr = freeo;
    mp->keytype = STRING_KEY;
    mp->cpa = string_compa;
    mp->em = my_calloc_ntype(KEY_SIZE, struct hmap_e *);
	int i;
	for(i = 0; i < mp->hsize; i++){
		mp->em[i] = NULL;
    }
	return mp;
}

/* 
 * function : 创建hashmap,指定hashmap的槽数
 * input    : none
 * output   : 创建好的空hashmap
 */
struct hmap *hmap_create_dt(int hsize, hfr fr, hcpa cpa, char keytype)
{
	struct hmap *mp = my_malloc_type(struct hmap);
	mp->size = 0;
    mp->hsize = hsize;
    if (fr == NULL)
        mp->fr = freeo;
    else
        mp->fr = fr;
    mp->keytype = keytype;
    if (cpa == NULL)
    {
       if (keytype == INT_KEY)
           mp->cpa = int_compa;
       else
           mp->cpa = string_compa;
    }else{
        mp->cpa = cpa;
    }
    mp->em = my_calloc_ntype(hsize, struct hmap_e *);
	int i;
	for (i = 0; i < mp->hsize; i++)
    {
		mp->em[i] = NULL;
    }
	return mp;
}

/* 
 * function : 逐个槽释放hashmap
 * input    : hashmap
 * output   : none
 */
void hmap_destroy(struct hmap *mp)
{
	int i;
	for (i = 0; i < mp->hsize; i++)
    {
        if (mp->em[i] != NULL)
    		mp->size -= hmap_key_destroy(mp->em[i], mp->fr);
    }
    free(mp->em);
    if (mp->size != 0)
        log_error("hmap size : %zd\n", mp->size);
	assert(mp->size == 0);
	free(mp);
}

struct hmap_e *hmap_put(struct hmap *mp, void *key, void *value)
{
	if(key == NULL)
        return NULL;
    //判断是否已经放入hashmap中，如果放入则替换内容,否则添加
	struct hmap_e *e  = hmap_get_e(mp, key);
	if (e != NULL)
	{   
        (*(mp->fr))(e->value);
        e->value = value;
        free(key);
		return e;
	}
    long h = hash_calc(mp->keytype, key) % (mp->hsize);
    //没有放入，则添加
	e = hmap_e_create(key, value);
    //把新元素放到每个key队列的头部
    e->next = mp->em[h];
    mp->em[h] = e;
    mp->size++;
    return e;
}

void *hmap_get(struct hmap *mp, void *key)
{   
    struct hmap_e *e = hmap_get_e(mp, key);
    if (e != NULL)
    {
        return e->value;
    }
	return NULL;
}


//删除目标key的节点
void *hmap_del(struct hmap *mp, void *key)
{
	if (key == NULL)
        return NULL;
    long h = hash_calc(mp->keytype, key) % (mp->hsize);
	struct hmap_e *e = mp->em[h];
    struct hmap_e *pre = hmap_e_create(NULL,NULL);
    pre->next = e;
	while (e)
	{
		if((*(mp->cpa))(key, e->key) == 0){
            pre->next = e->next;
            mp->size--;
            e->next = NULL;
            free(pre);
            //如果此槽中只剩下一个元素，pre的方式会不能删除掉
            if (mp->em[h] == e)
                mp->em[h] = NULL;
            void *r = e->value;
            free(e->key);
            free(e);
            return r;
        }
        pre = e;
        e = e->next;
	}
    free(pre);
	return NULL;
}

/* 
 * function : 打印hmap各个槽下边的数目，用于调试
 * input    : hmap
 * output   : stdout
 */
void hmap_h_print(struct hmap *mp){
   int i;
   for (i = 0; i < mp->hsize; i++){
       if (mp->em[i] == NULL)
           continue;
       struct hmap_e *e = mp->em[i];
       int count = 0;
       while (e){
           count++;
           e=e->next;
       }
       printf("em[%d]:%d  ", i, count);
       if (i % 50 == 0)
           printf("\n");
   } 
}

void hmap_print(struct hmap *mp)
{
	printf("hmap size :%d \n", (int)mp->size);
    int i;
	for(i = 0; i < mp->hsize; i++)
	{
		struct hmap_e *e= mp->em[i];
		while (e)
		{
            if (mp->keytype == STRING_KEY)
    			printf("%s:%s ", (char *)e->key, (char *)e->value);
            else
    			printf("%d:%s ", *((int *)e->key), (char *)e->value);
			e = e->next;
		}
	}
	printf("\n");
}

static int fakefree(void *t)
{
    return 1;
}
/* 
 *function : 生成指向hmap_e的list
 */
struct list* gethmapelist(struct hmap *mp)
{
    struct list *list = list_create_dt(fakefree);
    int i;
    for (i = 0; i < mp->hsize; i++)
    {
       if (mp->em[i] == NULL)
           continue;
       struct hmap_e *e = mp->em[i];
       while (e)
       {
           struct list_e *le = listnode_create(e);
           list_add(list, le);
           e = e->next;
       }
    }
    return list;
}

int hmap_remove(struct hmap *mp, void *key)
{
	long h = hash_calc(mp->keytype, key) % (mp->hsize);
	struct hmap_e *e = mp->em[h];
    if (e == NULL)
        return 0;
    if ((*(mp->cpa))(key, e->key) == 0)
    {
        mp->em[h] = e->next;
		free(e->key);
        (*(mp->fr))(e->value);
		free(e);
        mp->size--;
        return 1;
    }
       
    struct hmap_e *pre = e;
    e = e->next; 
	while (e)
	{
        if ((*(mp->cpa))(key, e->key) == 0)
        {
            pre->next = e->next;
            free(e->key);
            (*(mp->fr))(e->value);
            free(e);
            mp->size--;
            return 1;
        }
        pre = pre->next;
        e = e->next;
    }
    return 0;
}

