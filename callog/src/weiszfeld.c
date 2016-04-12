/*
 * =============================================================================
 *
 *       Filename:  weiszfeld.c
 *
 *    Description:  implementation of algorithm weiszfeld
 *        Version:  1.0
 *       Compiler:  gcc
 *
 *         Author:  quanzhao
 *   Organization:  tigerknows 
 *
 * =============================================================================
 */

#include "../include/weiszfeld.h"

/* Algorithm for caculating a center point from a list of points with weight.
 *
 * @ss contains all the sample-points, MAKE SURE that distances have been
 *  caculated before calling this function and havn't been freed.
 *
 * @stop_distance describe that algorithm stops when the distance between 
 *  result and last result of iterations less than @stop_distance. This 
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
struct coordinate *weiszfeld(struct samples *ss, double stop_distance)
{
    struct coordinate *result = coordinate_create('c', 0.0, 0.0);
    struct sample *sa;
    double n, rx, ry, wx, wy, w;
    // "x_tmp" and "y_tmp" used to save last result of iteration
    double x_tmp, y_tmp;
    // used to save the size of sample-points
    int num = 0;
    // get average of all coordinates
    samples_visit_rewind(ss);
    while ((sa = samples_visit(ss)) != NULL)
    {
        result->x += sa->co->x;
        result->y += sa->co->y;
        num++;
    }
    result->x /= num;
    result->y /= num;
    // iterates
    int loop = 0;
    while (true)
    {
        n = rx = ry = wx = wy = w = 0.0;
        // visit all sample-points and caculates temp value
        samples_visit_rewind(ss);
        while ((sa = samples_visit(ss)) != NULL)
        {
            struct coordinate *point = sa->co;
            double weight = sa->weight;
            if (result->x == point->x && result->y == point->y)
                    n += weight;
            else
            {
                double dis = cartesian_distance(point->x, point->y, result->x, result->y);
                double tmp = weight / dis;
                rx += tmp * (point->x - result->x);
                ry += tmp * (point->y - result->y);
                wx += tmp * point->x;
                wy += tmp * point->y;
                w += tmp;
            }
        }
        // special case when the algorithm ends itself (get optimized answer directlly)
        double r = cartesian_distance(rx, ry, 0.0, 0.0);
        if (n >= r)
            break;
        x_tmp = wx / w;
        y_tmp = wy / w;
        // iterated to a required answer
        bool flag = cartesian_distance(result->x, result->y, x_tmp, y_tmp) < stop_distance;
        result->x = x_tmp;
        result->y = y_tmp;
        if (flag)
            break;
        loop++;
        // iterates to a limit number
        if (loop > WEISZFELD_LOOP_LIMIT)
        {
            //printf("Algorithm weizsfeld loop too much!\n");
            break;
        }
    }
    return result;
}

