/*
 * =============================================================================
 *
 *       Filename:  weiszfeld.h
 *
 *    Description:  defination of algorithm weiszfeld
 *        Version:  1.0
 *       Compiler:  gcc
 *
 *         Author:  quanzhao
 *   Organization:  tigerknows 
 *
 * =============================================================================
 */

#include "common.h"
#include <math.h>
#include "sample.h"

#ifndef __WEISZFELD_H__
#define __WEISZFELD_H__

/********************************* DEFINATIONS ********************************/

/* The limit of loop counts when the algorithm iterates */
#define WEISZFELD_LOOP_LIMIT 10000

/****************************** EXTERN FUNCTIONS ******************************/

/* Algorithm for caculating a center point from a list of points with weight.
 *
 * @ss contains all the sample-points, MAKE SURE that distances have been
 *  caculated before calling this function and havn't been freed.
 *
 * @stop_distance describe that algorithm stops when the distance between 
 *  result and last result of iterations less than "stop_distance". This 
 *  algorithm is "MONOTONICALLY CONVERGENT" so that the distance between result
 *  and last result will be smaller after each iteration.
 *
 * "WEISZFELD_LOOP_LIMIT" is used in another case that the algorithm iterates
 *  too much, the algoritem stops when iterates more than "WEISZFELD_LOOP_LIMIT"
 *  times.
 *
 * ATTENTION that this algorithm is different from other simple method because
 *  it tries to caculate optimization of cost function "sum(w_i * d_i) for i", 
 *  the weight "w_i" effects on non-linear factor distance "d_i" NOT simple
 *  coordinates. BUT we can measure it's result and choose a good algorithm
 *  relatively.
 *
 * For more information please refer to documents or following web site:
 *  http://www.stat.rutgers.edu/home/cunhui/papers/43.pdf
 */
extern struct coordinate *
weiszfeld(struct samples *ss, double stop_distance);

#endif /* __WEISZFELD_H__ */

