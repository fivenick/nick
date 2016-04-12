/*
 * =============================================================================
 *
 *       Filename:  weiszfeld.c
 *
 *    Description:  testing
 *        Version:  1.0
 *       Compiler:  gcc
 *
 *         Author:  quanzhao
 *   Organization:  tigerknows 
 *
 * =============================================================================
 */

#include <stdio.h>
#include "../include/weiszfeld.h"

void main(int argc,char** argv)
{
/*    struct coordinate *c1 = coordinate_create('s', 116.588717, 40.0706);    
    struct coordinate *c2 = coordinate_create('s', 116.588717, 40.0706);
    struct sample *sa1 = sample_create(c1, 0, 0.0, 1.0);
    struct sample *sa2 = sample_create(c2, 0, 0.0, 1.0);
    struct samples *ss = samples_create();
*/
    struct coordinate *c1 = coordinate_create('c', 2, 0);    
    struct coordinate *c2 = coordinate_create('c', 4, 0);    
    struct coordinate *c3 = coordinate_create('c', 3, 1.73205);    
    struct sample *sa1 = sample_create(c1, 0, 0.0, 1.0);
    struct sample *sa2 = sample_create(c2, 0, 0.0, 1.0);
    struct sample *sa3 = sample_create(c3, 0, 0.0, 1.0);
    struct samples *ss = samples_create();
    int i = 0;
    samples_add(ss, sa1);
    samples_add(ss, sa2);
    samples_add(ss, sa3);
	if(argc < 2)
		return;
	struct samples *ss=getFileSamples(argv[1]);
    samples_visit_rewind(ss);
    struct coordinate *result = weiszfeld(ss, 1.0);
    printf("%.6f %.4f\n", result->x, result->y);
//    printf("Expect: 116.588717 40.0706\n");
    coordinate_destroy(result);
    samples_destroy(ss);
}
