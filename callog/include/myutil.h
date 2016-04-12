/*
 * =====================================================================================
 *
 *       Filename:  myutil.h
 *
 *    Description:  myutil头文件
 *
 *        Version:  1.0
 *        Created:  2012年09月14日 17时29分39秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  hepan
 *   Organization:  tigerknows 
 *
 * =====================================================================================
 */

#include "common.h"

#ifndef __MYUTIL_H__
#define __MYUTIL_H__

/********************************* DEFINATIONS ********************************/

/* size and length of time string with format "yyyy-mm-dd hh:mm:ss" */
#define SEC_TIME_STR_LENGTH 19
#define SEC_TIME_STR_SIZE 20
/* size and length of time string with format "yyyy-mm-dd hh:mm:ss.xxx" */
#define MILISEC_TIME_STR_LENGTH 23
#define MILISEC_TIME_STR_SIZE 24

/* uniseconds in one second */
#define UNISECS_SEC 1000000

/* Define our own malloc */
#define malloc "DO NOT CALL malloc DIRECTLLY!"
#define my_malloc(bytes)                                                       \
    my_malloc_noexit(bytes, __FILE__, __LINE__)
#define my_malloc_type(type)                                                   \
    (type *)my_malloc_noexit(sizeof(type), __FILE__, __LINE__)
#define my_malloc_ntype(num, type)                                             \
    (type *)my_malloc_noexit((num) * sizeof(type), __FILE__, __LINE__)
/* Define our own calloc */
#define calloc "DO NOT CALL calloc DIRECTLLY!"
#define my_calloc(num, size)                                                   \
    my_calloc_noexit(num, size, __FILE__, __LINE__)
#define my_calloc_type(type)                                                   \
    (type *)my_calloc_noexit(1, sizeof(type), __FILE__, __LINE__)
#define my_calloc_ntype(num, type)                                             \
    (type *)my_calloc_noexit(num, sizeof(type), __FILE__, __LINE__)
/* Define our own realloc */
#define realloc "DO NOT CALL realloc DIRECTLLY!"
#define my_realloc(old_mem, new_size)                                          \
    ((typeof(old_mem))my_realloc_noexit(old_mem, new_size, __FILE__, __LINE__))

/* Dereferance the void pointer(@voidp) to value of @type */
#define my_derefvp(voidp, type)                                                \
    (*((type *)(voidp)))
/* Extract left-value of @type from @voidp(void *) */
#define my_vp2type(voidp, type)                                                \
    (*((type *)&(voidp)))
/* Extract left-value of @type from any @pointer */
#define my_p2type(pointer, type)                                               \
    my_vp2type((void *)(pointer), type)

/* Offset of @MEMBER in struct @TYPE */
#define my_offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
/* Get the container-struct's pointer
 * @ptr pointer of member
 * @type type of container-struct
 * @member name of the member in struct
 */
#define my_container_of(ptr, type, member)                                     \
    ({                                                                         \
        const typeof( ((type *)0)->member ) *__mptr = (ptr);                   \
        (type *)( (char *)__mptr - offsetof(type, member) );                   \
    })

/* Get (char *)hostname-string of localhost
 * MAKE SURE to free the hostname-string */
#define my_get_hostname()                                                      \
    ({                                                                         \
        char buffer[MACHINE_CNF_HOSTNAME_MAXSIZE];                             \
        int ret = gethostname(buffer, MACHINE_CNF_HOSTNAME_MAXSIZE);           \
        if (ret != 0)                                                          \
            my_error_exit("gethostname(2): fail");                             \
        my_str_dump(buffer);                                                   \
    })

/* Get (char *)time-string of now
 * MAKE SURE to free the time-string */
#define my_get_timestr()                                                       \
    ({                                                                         \
        struct timeval nt;                                                     \
        gettimeofday(&nt, NULL);                                               \
        (char *)( my_get_str_from_mtime(nt.tv_sec, nt.tv_usec / 1000) );       \
    })

/* Catch signal @sig and do @action ignoring any other signals */
#define my_catch_sig_exclusive(sig, action)                                    \
    do                                                                         \
    {                                                                          \
        struct sigaction actSIG;                                               \
        actSIG.sa_handler = action;                                            \
        sigfillset(&actSIG.sa_mask);                                           \
        actSIG.sa_flags = 0;                                                   \
        sigaction(sig, &actSIG, 0);                                            \
    }                                                                          \
    while (false)

/* Trace function call when debuging */
#include <execinfo.h>
#define my_backtrace()                                                         \
    do                                                                         \
    {                                                                          \
        void *trace[256];                                                      \
        size_t trace_size = backtrace(trace, 256);                             \
        fprintf(stderr, "============= backtrace =============\n");            \
        backtrace_symbols_fd(trace, trace_size, fileno(stderr));               \
        fprintf(stderr, "=========== backtrace END ===========\n");            \
        fprintf(stderr, "For more information, refer to doc/TOOLS\n");         \
    }                                                                          \
    while (false)

/* Print message with file(line), use it as "printf" */
#define my_msg_report(...)                                                     \
    do                                                                         \
    {                                                                          \
        fprintf(stderr, "%s(%d): ", __FILE__, __LINE__);                       \
        fprintf(stderr, __VA_ARGS__);                                          \
    }                                                                          \
    while (false)
/* Print error message and function calls, then exit, use it as "printf" */
#define my_error_exit(...)                                                     \
    do                                                                         \
    {                                                                          \
        fprintf(stderr, "\nAborting.\n");                                      \
        fprintf(stderr, "ERROR in '%s(%d)': ", __FILE__, __LINE__);            \
        if (errno != 0)                                                        \
            fprintf(stderr, "%s", strerror(errno));                            \
        fprintf(stderr, "\n");                                                 \
        fprintf(stderr, __VA_ARGS__);                                          \
        fprintf(stderr, "\n");                                                 \
        my_backtrace();                                                        \
        exit(-1);                                                              \
    }                                                                          \
    while (false)

/* Macro for change value of @var1 and @var2 */
#define my_swap(var1, var2)                                                    \
    do                                                                         \
    {                                                                          \
        typeof(var1) tmp = var2;                                               \
        var2 = var1;                                                           \
        var1 = tmp;                                                            \
    }                                                                          \
    while (false)

/* Datas holds for a graffiti of string */
struct my_str_graffiti
{
    char *board;
    char *begin;
    char *end;
    char *now;
    int parts;
};

/* Init and return a string-graffiti copying the string and it is remained for
 *  further operations */
extern struct my_str_graffiti *
my_str_graffiti_init(char *str);

/* Free all resources malloced for string-graffiti(@msg) */
extern void 
my_str_graffiti_destroy(struct my_str_graffiti *msg);

/* Cut the string given(@str) in many parts according to any char in @delim,
 *  return a string-graffiti to save the cutted string */
extern struct my_str_graffiti *
my_str_tok_init(char *str, const char *delim);

/* Get Next-part of the cutted string from string-graffiti(@msg) */
extern char *
my_str_tok_get(struct my_str_graffiti *msg);

/* Get that there are how many parts in cutted string */
extern int 
my_str_tok_get_parts(struct my_str_graffiti *msg);

/****************************** EXTERN FUNCTIONS ******************************/

/* Our own malloc, exit while failed */
extern void *
my_malloc_exit(size_t bytes, char *file, int line);

/* Our own malloc, not exit while failed but write logs instead */
extern void *
my_malloc_noexit(size_t bytes, char *file, int line);

/* Our own calloc, not exit while failed but write logs instead */
extern void *
my_calloc_noexit(size_t num, size_t size, char *file, int line);

/* Our own realloc, not exit while failed but write logs instead */
extern void *
my_realloc_noexit(void *old_mem, size_t newsize, char *file, int line);

/* Write formated time string to @buffer with offset @off 
 *
 * @stime describe the seconds since "1900-01-01 00:00:00"
 * @mtime describe the mili-seconds left in [0, 999]
 * @return the new offset excluding '\0', MAKE SURE to add '\0' if this is the
 *  last step
 */
extern int 
my_write_mtime(char *buffer, int off, long stime, int mtime);

/* Get the formated time string 
 *
 * @stime describe the seconds since "1900-01-01 00:00:00"
 * @mtime describe the mili-seconds left in [0, 999]
 * @return malloced time string, MAKE SURE to free it when do not need it
 */
extern char *
my_get_str_from_mtime(long stime, int mtime);

/* Get long from time string in format "yyyy-mm-dd hh:mm:ss?", '?' is not num */
extern long 
my_get_long_from_timestr(char* timestr);

/*
 * function : 根据两个指针之间的内容，生成新的字符串，新申请的空间，需要free
 * input    : 两个字符指针，start,end;start指向需要内容的第一个字符，end指向末尾字符的后边一个空间
 * output   : 返回新生成的字符串，malloc了空间，需要free
 */
extern char *
createstr(char *start, char *end);

/*
 * function : 根据字符串，生成新的字符串，新申请的空间，需要free
 * input    : 字符串指针 str
 * output   : 返回新生成的字符串，malloc了空间，需要free
 */
extern char *
my_str_dump(char *str);

/* 
 * function : 查找第一个非空格字符的位置
 * input    : 用于查找的字符串
 * output   : 第一个非空格字符的位置，若全为空格字符则返回NULL
 */
extern char *
my_str_first_not_space(char *str);

/* 
 * function : 查找第一个空格字符的位置
 * input    : 用于查找的字符串
 * output   : 第一个空格字符的位置，若全为非空格字符则返回NULL
 */
extern char *
my_str_first_space(char *str);

/* 
 * function : 截取一个字符串的子串，转化为int
 * input    : char *ori 原始字符串，int from 字串起始位置，int to 字串结束位置
 * output   : 对应的整数
 */
extern int 
getint(char *ori, int from , int to);

/* 
 * funciton : 获得当前时间 
 * input    : 无
 * output   : 字符串返回
 */
extern char *
getnowtime();

/* 
 * function : 拼接多个字符串
 * input    : 拼接字符串的个数(n_values)，需要拼接的多个字符串，按参数位置拼接
 * output   : 拼好的新申请空间存放的字符串
 */
extern char *
my_nstr_cat(size_t n_values, ...);

/* 
 * function : 拼接两个字符串
 * input    : 需要拼接的两个字符串，按参数位置拼接
 * output   ：拼好的新申请空间存放的字符串
 */
extern char *
mystrcat(char *s1, char *s2);

/*
 * function : 得到整数的位数
 * input    : 要计算的数字
 * output   : 得到的位数
 */
extern int getdigitnum(int i);

/* 
 * function : 整数转化为字符串
 * input    : 需要转化的整数
 * output   : 结果字符串
 */
extern char *
myitoa(int i);

extern void *
memcat(void *block1, int size1, void *block2, int size2);

extern inline int 
str_endwith(char *str, char c);

extern char *
my_pathcat(char *path1, char *path2);

extern int
is_match(char *s,char *reg);

extern int
hashcode_s(void *s);
#endif /* __MYUTIL_H__ */

