/*
 * =============================================================================
 *
 *       Filename:  heap_sort.h
 *
 *    Description:  implementation of heap sort
 *        Version:  1.0
 *       Compiler:  gcc
 *
 *         Author:  quanzhao
 *   Organization:  tigerknows 
 *
 * =============================================================================
 */

#include "../include/heapsort.h"

/******************************** SOUCE CODES *********************************/

/* 1. index "0" is the pointer of root element.
 * 2. left-child of i is (2i + 1), right-child of i is 2(i + 1).
 * 3. father of i is (i - 1) / 2.
 */

/* Heapify @sort_array once in indexs ranged in [root_index, max_heap_index] */
inline void max_heapify(void **sort_array, int rootIndex, int maxHeapIndex, fun_hsort_comp comp)
{
    void *tmp;
    int lChild = rootIndex * 2 + 1;
    int rChild = (rootIndex + 1) * 2;
    int largest = rootIndex;
    if (lChild <= maxHeapIndex && (*comp)(sort_array[lChild], sort_array[rootIndex]) > 0)
        largest = lChild;
    if (rChild <= maxHeapIndex && (*comp)(sort_array[rChild], sort_array[largest]) > 0)
        largest = rChild;
    if (largest != rootIndex)
    {
        // swap pointers in sort_array[largest] and sort_array[rootIndex]
        my_swap(sort_array[largest], sort_array[rootIndex]);
        max_heapify(sort_array, largest, maxHeapIndex, comp);
    }
}

/* Heap sort @sort_array with @length, only @avalnum (void *) from head are 
 *  sequential*/
void heap_sort(void **sort_array, int len, int avalnum, fun_hsort_comp comp)  
{  
    if (len < 2 || avalnum <= 0)
        return;
    if (avalnum > len)
        avalnum = len;
    int i = 0;  
    int maxHeapIndex = avalnum - 1;  
    int maxIndex = len - 1;
    // create heap 
    for (i = (maxHeapIndex - 1) / 2; i >= 0; i--)  
    {  
        max_heapify(sort_array, i, maxHeapIndex, comp);  
    }  
    for (i = avalnum; i <= maxIndex; i++)
    {
        if ((*comp)(sort_array[0], sort_array[i]) > 0)
        {
            my_swap(sort_array[0], sort_array[i]);
            max_heapify(sort_array, 0, maxHeapIndex, comp);
        }
    }
    // out heap
    for (i = maxHeapIndex; i > len - avalnum; i--)  
    {  
        // swap pointers in sort_array[0] and sort_array[i]
        my_swap(sort_array[0], sort_array[i]);
        // continue to heapify
        max_heapify(sort_array, 0, i - 1, comp);  
    }  
    if (avalnum != len)
    {
        // swap pointers in sort_array[0] and sort_array[i]
        my_swap(sort_array[0], sort_array[i]);
    }
}  

/* For parsing to heap_sort */
int heapsort_double_comp_asc(void *item1, void *item2)
{
    double d1 = *((double *)item1);
    double d2 = *((double *)item2);
    if (d1 > d2)
        return 1;
    else if (d1 < d2)
        return -1;
    return 0;
}

/* For parsing to heap_sort */
int heapsort_double_comp_dsc(void *item1, void *item2)
{
    double d1 = *((double *)item1);
    double d2 = *((double *)item2);
    if (d1 < d2)
        return 1;
    else if (d1 > d2)
        return -1;
    return 0;
}

