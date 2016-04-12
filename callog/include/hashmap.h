/*
 * =====================================================================================
 *
 *       Filename:  hashmap.h
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
 *   Summary：一种依key类型而确定Hash算法的 链式防碰的哈希表
 *
 * =====================================================================================
 */

#include "common.h"

#ifndef __HASHMAP_H__
#define __HASHMAP_H__

#define KEY_SIZE 1024

//释放hmap中value的回调函数
typedef int (*hfr)(void *);

typedef int (*hcpa)(void *, void *);

struct hmap_e
{
	void *key;
	void *value;
	struct hmap_e *next; 
};

struct hmap
{
    hfr fr;//释放value的回调函数
    hcpa cpa;//用于比较key值是否相等的比较函数指针
	struct hmap_e **em;
    size_t size;
    unsigned int hsize;
    char keytype;
};

static long hash_string(const char *key);

static long hash_int(const int *key);

static int hmap_key_destroy(struct hmap_e *e, hfr fr);

static long hash_calc(const char keytype, void *key);

extern struct hmap_e *hmap_get_e(struct hmap *mp, void *key);

extern struct hmap_e *hmap_e_create(void *key, void *value);

extern struct hmap* hmap_create();

extern struct hmap* hmap_create_dt(int hsize, hfr fr, hcpa cpa, char keytype);

extern void hmap_destroy(struct hmap *mp);

extern struct hmap_e *hmap_put(struct hmap *mp, void *key, void *value);

extern void *hmap_get(struct hmap *mp, void *key);

extern void *hmap_del(struct hmap *mp, void *key);

extern struct list *hmap_valuelist(struct hmap *mp);

extern void printHsize(struct hmap *mp);

extern void hmap_print(struct hmap *mp);

extern struct list* gethmapelist(struct hmap *mp);

extern int hmap_calcsize(struct hmap *mp);

extern int hmap_remove(struct hmap *mp, void *key);

#endif /*__HASHMAP_H__*/

