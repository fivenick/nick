/*
 * =====================================================================================
 *
 *       Filename:  t_file.h
 *
 *    Description:  封装文件操作的头文件
 *
 *        Version:  1.0
 *        Created:  2013年04月25日 16时26分01秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  hepan
 *   Organization:  tigerknows
 *
 * =====================================================================================
 */

#include "common.h"

#ifndef __T_FILE_H__
#define __T_FILE_H__

#define RW_COUNT 1000000
#define PATH_BUFFER_SIZE 1024

extern long 
t_fsize(FILE *fp);

extern int 
f_getpath(int fd, char pathname[], int n);

extern FILE *
t_fopen(char *path, char *md);

extern int 
t_fread(void *buffer, int size, int count, FILE *fp);

extern int
t_fclose(FILE *fp);

extern int 
t_fwrite(void *buffer, int size, int count, FILE *fp);

extern int
t_fputs(const char *str, FILE *fp);

extern char *
t_fgets(char *str, int size, FILE *fp);

extern inline int
t_fseek(FILE *fp, long offset, int fromwhere);
#endif /* __T_FILE_H__ */

