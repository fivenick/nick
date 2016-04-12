/*
 * =============================================================================
 *
 *       Filename:  sample.h
 *
 *    Description:  defination of sample-points
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

#ifndef __SAMPLE_H__
#define __SAMPLE_H__

/********************************* DEFINATIONS ********************************/

/* Radius of the earth, SI = m(meter) */
#define EARTH_RADIUS 6371004.0
/* Circle ratio */
#define PI 3.14159265358979323846
#define SAMPLE_COOR_D2I_FACTOR 1000000

/* Struct for saving a coordinate */
struct coordinate
{
    double x;   // longitude or x in cartesian
    double y;   // latitude or y in cartesian
    char type;  // 's' means spherical and 'c' means cartesian
};

/* Defination of sample-point */
struct sample
{
    double precision;
    double weight;
    long time;              // miliseconds counts from the year 1970
    struct coordinate *co;  // MAKE SURE to create a coordinate for this pointer
};

/* Pointer of function used as a comparator for sample-point 
 *
 * Suggest to USE "sample_comp_time_asc" or "sample_comp_time_dsc".
 */
typedef int (*fun_sample_comp)(void *sa1, void *sa2);

/* Defination of sample-points and their distances 
 * 
 * MAKE SURE you have called the function "samples_caculate_distances[_fast]"
 *  before you want to use function "samples_get_distance" to get the distance
 *  which have been caculated and saved.
 *
 * Don't worry about the memories malloced, "samples_caculate_distances[_fast]"
 *  will handled for you. DO REMEMBER to called function "samples_destroy" to
 *  free all related memories malloced for struct samples when you don't need
 *  these sample-points any more.
 */
struct samples
{
    struct list *data;          // data of sample-points
    struct list_e *now_pointer; // used for quickly visiting the list
    double *distances;          // save the distances of every pair of points
	long *timestamp;            //保存每个点的时间戳
};

/****************************** EXTERN FUNCTIONS ******************************/

/*----------------------------- part coordinate ------------------------------*/

/* Create and hold data for a coordinate */
extern struct coordinate *
coordinate_create(char type, double x, double y);

extern struct coordinate *
coordinate_create_int(char type, int x, int y);
/* Free all memories malloced for a coordinate */
extern void 
coordinate_destroy(struct coordinate *c);

/* Parse a coorinate in spherical to a coordinate in cartesian */
extern void
to_cartesian(struct coordinate *center, struct coordinate *c);

/* Parse a coorinate in cartesian to a coordinate in spherical */
extern void
to_spherical(struct coordinate *center, struct coordinate *c);

/* Caculate cartesian distance */
extern double
cartesian_distance(double x1, double y1, double x2, double y2);

/* Use spherical coordinate to caculate distance between two points.
 *
 * @lon1 and @lat1 describe the longitude and latitude for point 1 and so do
 *  @lon2 and @lat2.
 *
 * Algorithm will change all coordinates to radian by dividing (PI / 180), and
 *  then caculate a temp value: 
 *  "a = pow(sin((rlat2 - rlat1) / 2), 2) 
 *      + pow(sin(drlon / 2), 2) * cos(rlat1) * cos(rlat2)",
 *  then caculate result "2 * EARTH_RADIUS * atan2(sqrt(a), sqrt(1 - a))".  
 */
extern double
spherical_distance_norm(double lon1, double lat1, double lon2, double lat2);

/* Use spherical coordinate to caculate distance between two points.
 *
 * MORE EFFICIANCY!!! We have proved that this algorithm is equivalence 
 *  precisely with algorithm "spherical_distance_norm".
 *
 * @lon1 and @lat1 describe the longitude and latitude for point 1 and so do
 *  @lon2 and @lat2.
 *
 * Algorithm will change all coordinates to radian by dividing (PI / 180), and
 *  then caculate result:
 *  "EARTH_RADIUS * acos( cos(rlat1) * cos(rlat2) * cos(rlon1-rlon2)
 *      + sin(rlat1) * sin(rlat2) )"
 */
extern double
spherical_distance_fast(double lon1, double lat1, double lon2, double lat2);

/* Use encapsulated coordinate to caculate distance between two points.
 * 
 * The function will call "spherical_distance_fast" after parsing c1 and c2
 *  to propriate spherical coordiates.
 */
extern double
spherical_distance_co(struct coordinate *c1, struct coordinate *c2);

/*-------------------------------- part sample -------------------------------*/

/* Create and hold data for a sample-point.
 *
 * MAKE SURE @co is only belong to this sample-point. The management of @co
 *  will be parsing to sample-point so you need not to worry about freeing
 *  @co, just call "sample_destroy" to free all memories after using.
 */
extern struct sample *
sample_create(struct coordinate *co, long time, double precision, double weight);

/* Free all memories malloced for a sample-point */
extern void 
sample_destroy(struct sample *sa);

/* Comparator for field "time" of sample-point by ascending order */
extern int 
sample_comp_time_asc(void *sa1, void *sa2);

/* Comparator for field "time" of sample-point by descending order */
extern int 
sample_comp_time_dsc(void *sa1, void *sa2);

/*-------------------------------- part samples ------------------------------*/

/* Create and hold data for a list of sample-points, initial empty */
extern struct samples *
samples_create();

/* Free all memories malloced for a list of sample-point */
extern void 
samples_destroy(struct samples *ss);

/* Add element to the list of sample-points.
 *
 * MAKE SURE @sa is only belong to this list of sample-points @ss. The 
 *  management of @sa will be parsing to @ss so you need not to worry about
 *  freeing @sa, just call "samples_destroy" to free all memories after using.
 */
extern void 
samples_add(struct samples *ss, struct sample *sa);

/* Get size of the list of sample-points */
extern size_t
samples_size(struct samples *ss);

/* Sort the samples according to comparator "comp" */
extern void 
samples_sort(struct samples *ss, fun_sample_comp comp);

/* Only reserve sample-points according to @need[].
 * 
 * MAKE SURE array @need[] have elements more than "samples_size(@ss)".
 */
extern void 
samples_shrink(struct samples *ss, bool *need);

/* Rewind "now_pointer" to the head of list of @ss */
extern void
samples_visit_rewind(struct samples *ss);

/* Visit and return the element "struct sample" according to pointer "now_pointer" 
 *  and then pointer to next element 
 */
extern struct sample *
samples_visit(struct samples *ss);

/* Caculate all distances between each pair of sample-points in @ss.
 *
 * This is a stable and square method! BUT with NO optimization! 
 *
 * You should not worry about the memories malloced for saving distances,
 *  just use "samples_get_distance" to get the result you want.
 */
extern void 
samples_caculate_distances(struct samples *ss);

/* Caculate all distances between each pair of sample-points in @ss.
 *
 * This is a OPTIMIZED method with fast speed!!! Twice faster when comparing to
 *  function "samples_caculate_distances".
 *
 * You should not worry about the memories malloced for saving distances,
 *  just use "samples_get_distance" to get the result you want.
 */
extern void 
samples_caculate_distances_fast(struct samples *ss);

/* Get the distance of point with @index1 and point with @index2.
 *
 * MAKE SURE that function "samples_caculate_distances[_fast]" has been called.
 */
extern double 
samples_get_distance(struct samples *ss, size_t index1, size_t index2);

/* Set the distance of point with "index1" and point with @index2 to @value.
 *
 * This function is NOT suggested to use!!!
 *
 * MAKE SURE that function "samples_caculate_distances[_fast]" has been called.
 */
extern void 
samples_set_distance(struct samples *ss, size_t index1, size_t index2, double value);

# endif /* __SAMPLE_H__ */

