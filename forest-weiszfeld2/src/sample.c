/*
 * =============================================================================
 *
 *       Filename:  sample.h
 *
 *    Description:  implements for coordinate, sample and samples
 *        Version:  1.0
 *       Compiler:  gcc
 *
 *         Author:  quanzhao
 *   Organization:  tigerknows 
 *
 * =============================================================================
 */

#include "../include/sample.h"

/***************************** INTERNAL FUNCTIONS *****************************/

static int
sample_destroy_dt(void *sample);

static void
samples_create_distances(struct samples *ss);

static void
samples_clear_distances(struct samples *ss);

static void
samples_clear_timestamp(struct samples *ss);

/****************************** STATIC DATA PART ******************************/

/* For caculate more faster */
static const double ANGLE_TO_RADIAN = PI / 180;
static const double ANGLE_TO_METER = EARTH_RADIUS * PI / 180;
static struct coordinate dfcenter = {0.0, 0.0, 's'};

/******************************** SOUCE CODES *********************************/

/* Create and hold data for a coordinate */
inline struct coordinate *coordinate_create(char type, double x, double y)
{
    struct coordinate *c = my_malloc_type(struct coordinate);
    assert(type == 's' || type == 'c');
    c->type = type;
    c->x = x;
    c->y = y;
    return c;
}

/* Create and hold data for a coordinate with int input */
inline struct coordinate *coordinate_create_int(char type, int x, int y)
{
    struct coordinate *c = my_malloc_type(struct coordinate);
    assert(type == 's' || type == 'c');
    c->type = type;
    c->x = ((double) x) / SAMPLE_COOR_D2I_FACTOR;
    c->y = ((double) y) / SAMPLE_COOR_D2I_FACTOR;
    return c;
}

/* Free all memories malloced for a coordinate */
inline void coordinate_destroy(struct coordinate *c)
{
    free(c);
}

/* Parse a coorinate in spherical to a coordinate in cartesian */
inline void to_cartesian(struct coordinate *center, struct coordinate *c)
{
    assert(c->type == 's');
    c->x = (c->x - center->x) * ANGLE_TO_METER * cos(center->y * ANGLE_TO_RADIAN);
    c->y = (c->y - center->y) * ANGLE_TO_METER;
    c->type = 'c';
}

/* Parse a coorinate in cartesian to a coordinate in spherical */
inline void to_spherical(struct coordinate *center, struct coordinate *c)
{
    assert(c->type == 'c');
    c->x = c->x / ANGLE_TO_METER / cos(center->y * ANGLE_TO_RADIAN) + center->x;
    c->y = c->y / ANGLE_TO_METER + center->y;
    c->type = 's';
}

/* Caculate cartesian distance */
inline double cartesian_distance(double x1, double y1, double x2, double y2)
{
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

/* Use encapsulated coordinate to caculate distance between two points.
 * 
 * The function will call "spherical_distance_fast" after parsing c1 and c2
 *  to propriate spherical coordiates.
 */
inline double spherical_distance_co(struct coordinate *c1, struct coordinate *c2)
{
    if (c1->type == 'c')
        to_spherical(&dfcenter, c1);
    if (c2->type == 'c')
        to_spherical(&dfcenter, c2);
    assert(c1->type == 's');
    assert(c2->type == 's');
    return spherical_distance_fast(c1->x, c1->y, c2->x, c2->y);
}

/* Use spherical coordinate to caculate distance between two points.
 *
 * @lon1 and @lat1 describe the longitude and latitude for point 1 and so do
 *  @lon2 and @lat2.
 *
 * Algorithm will change all coordinates to radian by dividing (PI / 180), and
 *  then caculate a temp value: 
 *  "a = pow(sin((rlat2 - rlat1) / 2), 2) 
 *    + pow(sin(drlon / 2), 2) * cos(rlat1) * cos(rlat2)",
 *  then caculate result "2 * EARTH_RADIUS * atan2(sqrt(a), sqrt(1 - a))".  
 */
inline double spherical_distance_norm(double lon1, double lat1, double lon2, double lat2)
{
    double rlat1 = lat1 * ANGLE_TO_RADIAN;
    double rlat2 = lat2 * ANGLE_TO_RADIAN;
    double drlat = rlat2 - rlat1;
    double drlon = (lon2 - lon1) * ANGLE_TO_RADIAN;
    double a = pow(sin(drlat / 2), 2) + pow(sin(drlon / 2), 2) * cos(rlat1) * cos(rlat2);
    return 2 * EARTH_RADIUS * atan2(sqrt(a), sqrt(1 - a));  
}

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
 *    + sin(rlat1) * sin(rlat2) )"
 */
inline double spherical_distance_fast(double lon1, double lat1, double lon2, double lat2)
{
    double rlon1 = lon1 * ANGLE_TO_RADIAN;  
    double rlat1 = lat1 * ANGLE_TO_RADIAN;
    double rlon2 = lon2 * ANGLE_TO_RADIAN;
    double rlat2 = lat2 * ANGLE_TO_RADIAN;
    return EARTH_RADIUS * acos(cos(rlat1) * cos(rlat2) * cos(rlon1-rlon2) + sin(rlat1) * sin(rlat2));
}

/* Create and hold data for a sample-point.
 *
 * MAKE SURE @co is only belong to this sample-point. The management of @co
 *  will be parsing to sample-point so you need not to worry about freeing
 *  @co, just call "sample_destroy" to free all memories after using.
 */
inline struct sample *sample_create(struct coordinate *co, long time, double precision, double weight)
{
    struct sample *sa = my_malloc_type(struct sample);
    sa->co = co;
    sa->time = time;
    sa->precision = precision;
    sa->weight = weight;
    return sa;
}

/* Free all memories malloced for a sample-point */
inline void sample_destroy(struct sample *sa)
{
    coordinate_destroy(sa->co);
    free(sa);
}

/* Special destroyer for parsing to list */
inline int sample_destroy_dt(void *sample)
{
    sample_destroy((struct sample *)sample);
    return 0;
}

/* Comparator for field "time" of sample-point by ascending order */
inline int sample_comp_time_asc(void *sa1, void *sa2)
{
    long time1 = ((struct sample *)sa1)->time;
    long time2 = ((struct sample *)sa2)->time;
    return time1 == time2 ? 0 : time1 < time2 ? -1 : 1;
}

/* Comparator for field "time" of sample-point by descending order */
inline int sample_comp_time_dsc(void *sa1, void *sa2)
{
    long time1 = ((struct sample *)sa1)->time;
    long time2 = ((struct sample *)sa2)->time;
    return time1 == time2 ? 0 : time1 < time2 ? 1 : -1;
}

/* Create and hold data for a list of sample-points, initial empty */
struct samples *samples_create()
{
    struct samples *ss = my_malloc_type(struct samples);
    ss->data = list_create_dt(sample_destroy_dt);
    ss->now_pointer = NULL;
    ss->distances = NULL;
	ss->timestamp = NULL;
    return ss;
}

/* Free all memories malloced for a list of sample-point */
void samples_destroy(struct samples *ss)
{
    samples_clear_distances(ss);
	samples_clear_timestamp(ss);
    list_destroy(ss->data);
    free(ss);
}

/* Add element to the list of sample-points.
 *
 * MAKE SURE @sa is only belong to this list of sample-points @ss. The 
 *  management of @sa will be parsing to @ss so you need not to worry about
 *  freeing @sa, just call "samples_destroy" to free all memories after using.
 */
inline void samples_add(struct samples *ss, struct sample *sa)
{
    struct list_e *item = listnode_create(sa);
    list_add(ss->data, item);
}

/* Get size of the list of sample-points */
inline size_t samples_size(struct samples *ss)
{
    return list_size(ss->data);
}

/* Sort the samples according to comparator @comp */
inline void samples_sort(struct samples *ss, fun_sample_comp comp)
{
    list_heap_sort(ss->data, comp);
}

/* Only reserve sample-points according to @need[].
 * 
 * MAKE SURE array @need[] have elements more than "samples_size(@ss)".
 */
inline void samples_shrink(struct samples *ss, bool *need)
{
    list_shrink(ss->data, need);
}

/* Rewind "now_pointer" to the head of list of @ss */
void samples_visit_rewind(struct samples *ss)
{
    ss->now_pointer = ss->data->head;
}

/* Visit and return the element "struct sample" according to pointer "now_pointer" 
 *  and then pointer to next element 
 */
inline struct sample *samples_visit(struct samples *ss)
{
    struct sample *result;
    // visited to the end of list
    if (ss->now_pointer == NULL)
        return NULL;
    result = (struct sample *)ss->now_pointer->data;
    ss->now_pointer = ss->now_pointer->next;
    return result;
}

/* Malloc enough memories for saving distances later */
void samples_create_distances(struct samples *ss)
{
    size_t ss_size = samples_size(ss);
    ss->distances = my_malloc_ntype(ss_size * ss_size, double);
}

void samples_create_timestamp(struct samples *ss)
{
	size_t ss_size = samples_size(ss);
	ss->timestamp=my_malloc_ntype(ss_size,long);
}

/* Free memories for saving distances */
void samples_clear_distances(struct samples *ss)
{
    if (ss->distances != NULL)  
    {
        free(ss->distances);
        ss->distances = NULL;
    }
}

void samples_clear_timestamp(struct samples *ss)
{
    if (ss->timestamp != NULL)
    {
        free(ss->timestamp);
        ss->timestamp = NULL;
    }
}

/* Caculate all distances between each pair of sample-points in @ss.
 *
 * This is a stable and square method! BUT with NO optimization! 
 *
 * Internally it will call function "spherical_distance_co" to caculate each
 *  distance. 
 *
 * You should not worry about the memories malloced for saving distances,
 *  just use "samples_get_distance" to get the result you want.
 */
void samples_caculate_distances(struct samples *ss)
{
    size_t ss_size = samples_size(ss);
    int i,j;
    struct list_e *si, *sj;
    double dis;
    samples_clear_distances(ss);
    samples_create_distances(ss);
    si = sj = ss->data->head;
    for (i = 0; i < ss_size; i++)
    {
        for (j = i + 1; j < ss_size; j++)
        {
            sj = sj->next;  
            dis = spherical_distance_co(((struct sample *)si->data)->co, ((struct sample *)sj->data)->co);
            // change distance to a propriate value
            dis -= ((struct sample *)si->data)->precision + ((struct sample *)sj->data)->precision;
            if (dis > 0)
            {
                samples_set_distance(ss, i, j, dis);
                samples_set_distance(ss, j, i, dis);
            }
            else
            {
                samples_set_distance(ss, i, j, 0.0);
                samples_set_distance(ss, j, i, 0.0);
            }
        }
        samples_set_distance(ss, i, i, 0.0);
        si = sj = si->next;
    }
}

void samples_calculate_timestamp(struct samples *ss)
{
	 size_t ss_size = samples_size(ss);
	 int i=0;
	 samples_clear_timestamp(ss);
	 samples_create_timestamp(ss);
	 struct list_e *e=ss->data->head;
	 for (i=0;i<ss_size;i++)
	 {
		 ss->timestamp[i]=((struct sample *)e->data)->time;
		 e=e->next;
	 }
}

/* Caculate all distances between each pair of sample-points in @ss.
 *
 * This is a OPTIMIZED method with fast speed!!! Twice faster when comparing to
 *  function "samples_caculate_distances".
 *
 * It uses tricks to reduce some caculations of O(n^2) to O(n) but the main
 *  complexity is also O(n^2).
 *
 * You should not worry about the memories malloced for saving distances,
 *  just use "samples_get_distance" to get the result you want.
 */
void samples_caculate_distances_fast(struct samples *ss)
{
    size_t ss_size = samples_size(ss);
    int i,j;
    struct list_e *si, *sj;
    struct sample *sa;
    double x, y;
    double rx, ry;
    double sinx, cosx, siny[ss_size], cosy, sinxcosy[ss_size], cosxcosy[ss_size];
    double dis;
    samples_clear_distances(ss);
    samples_create_distances(ss);
    // caculate temp value of every point for fast caculating
    si = ss->data->head;
    for (i = 0; i < ss_size; i++)
    {
        sa = (struct sample *)si->data;
        rx = sa->co->x * ANGLE_TO_RADIAN;
        ry = sa->co->y * ANGLE_TO_RADIAN;
        sinx = sin(rx);
        cosx = sqrt(1 - pow(sinx, 2));
        siny[i] = sin(ry);  
        cosy = sqrt(1 - pow(siny[i], 2));
        sinxcosy[i] = sinx * cosy;
        cosxcosy[i] = cosx * cosy;
        si = si->next;
    }
    // fast caculate distance 
    si = sj = ss->data->head;
    for (i = 0; i < ss_size; i++)
    {
        for (j = i + 1; j < ss_size; j++)
        {
            sj = sj->next;
            dis = EARTH_RADIUS * acos(sinxcosy[i] * sinxcosy[j] + cosxcosy[i] * cosxcosy[j] + siny[i] * siny[j]);
            // change distance to a propriate value
            dis -= ((struct sample *)si->data)->precision + ((struct sample *)sj->data)->precision;
            if (dis > 0)
            {
                samples_set_distance(ss, i, j, dis);
                samples_set_distance(ss, j, i, dis);
            }
            else
            {
                samples_set_distance(ss, i, j, 0.0);
                samples_set_distance(ss, j, i, 0.0);
            }
        }
        samples_set_distance(ss, i, i, 0.0);
        si = sj = si->next;
    }
}

/* Get the distance of point with @index1 and point with @index2.
 *
 * MAKE SURE that function "samples_caculate_distances[_fast]" has been called.
 */
inline double samples_get_distance(struct samples *ss, size_t index1, size_t index2)
{
    return (ss->distances)[samples_size(ss) * index1 + index2];
}

inline long samples_get_timestamp(struct samples *ss, size_t index)
{
	return (ss->timestamp)[index];
}

/* Set the distance of point with @index1 and point with @index2 to @value.
 *
 * This function is NOT suggested to use!!!
 *
 * MAKE SURE that function "samples_caculate_distances[_fast]" has been called.
 */

inline void samples_set_distance(struct samples *ss, size_t index1, size_t index2, double value)
{
    (ss->distances)[samples_size(ss) * index1 + index2] = value;
}
