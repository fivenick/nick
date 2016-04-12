/*
 * =============================================================================
 *
 *       Filename:  heap_sort.h
 *
 *    Description:  defination of heap sort
 *        Version:  1.0
 *       Compiler:  gcc
 *
 *         Author:  quanzhao
 *   Organization:  tigerknows 
 *
 * ============================int===========================================
 */

#include "common.h"

#ifndef __HEAP_SORT_H__
#define __HEAP_SORT_H__

/********************************* DEFINATIONS ********************************/

/* Compare @item1 and @item2.
 * 
 * @Return "0" if @item1 is equal to @item2.
 * @Return "1" if @item1 will be put after @item2.
 * @Return "-1" if @item1 will be put before @item2.
 */
typedef int (*fun_hsort_comp)(void *item1, void *item2);

/****************************** EXTERN FUNCTIONS ******************************/

/* Heap sort @sort_array with @length, only @avalnum (void *) from head are 
 *  sequential */
extern void 
heap_sort(void **sort_array, int length, int avalnum, fun_hsort_comp comp);

/* Heapify @sort_array once in indexs ranged in [root_index, max_heap_index] */
extern void 
max_heapify(void **sort_array, int root_index, int max_heap_index, fun_hsort_comp comp);

/* For parsing to heap_sort */
extern int 
heapsort_double_comp_asc(void *item1, void *item2);

/* For parsing to heap_sort */
extern int
heapsort_double_comp_dsc(void *item1, void *item2);

#endif /* __HEAP_SORT_H__ */

