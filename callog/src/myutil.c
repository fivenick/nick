/*
 * =====================================================================================
 *
 *       Filename:  myutil.c
 *
 *    Description:  常用util
 *
 *        Version:  1.0
 *        Created:  2012年09月14日 17时28分04秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  hepan
 *   Organization:  tigerknows 
 *
 * =====================================================================================
 */

#include "../include/myutil.h"

/* Cancel macro "malloc" "calloc" "realloc" defined in common.h */
#undef malloc
#undef calloc
#undef realloc

/* Our own malloc, exit while failed */
inline void *my_malloc_exit(size_t bytes, char *file, int line)
{
    void *new_mem = malloc(bytes);
    if (new_mem == NULL)
        my_error_exit("Out of memmory when malloc: %s(%d)!", file, line);
    return new_mem;
}

/* Our own malloc, not exit while failed but write logs instead */
inline void *my_malloc_noexit(size_t bytes, char *file, int line)
{
    void *new_mem = malloc(bytes);
    if (new_mem == NULL)
    {
        my_backtrace();
        // more information printed to standard error to avoid error in log system
        fprintf(stderr, "Out of memmory when malloc: %s(%d)!", file, line);
        fflush(stdout);
        fflush(stderr);
        // write log
        log_error("Out of memmory when malloc: %s(%d)!", file, line);
    }
    return new_mem;
}

/* Our own calloc, not exit while failed but write logs instead */
inline void *my_calloc_noexit(size_t num, size_t size, char *file, int line)
{
    void *new_mem = calloc(num, size);
    if (new_mem == NULL)
    {
        my_backtrace();
        // more information printed to standard error to avoid error in log system
        fprintf(stderr, "Out of memmory when malloc: %s(%d)!", file, line);
        fflush(stdout);
        fflush(stderr);
        // write log
        log_error("Out of memmory when calloc: %s(%d)!", file, line);
    }
    return new_mem;
}

/* Our own realloc, not exit while failed but write logs instead */
inline void *my_realloc_noexit(void *old_mem, size_t newsize, char *file, int line)
{
    void *new_mem = realloc(old_mem, newsize);
    if (new_mem == NULL)
    {
        my_backtrace();
        // more information printed to standard error to avoid error in log system
        fprintf(stderr, "Out of memmory when malloc: %s(%d)!", file, line);
        fflush(stdout);
        fflush(stderr);
        // write log
        log_error("Out of memmory when realloc: %s(%d)!", file, line);
    }
    return new_mem;
}

/* Init and return a string-graffiti copying the string and it is remained for
 *  further operations */
inline struct my_str_graffiti *my_str_graffiti_init(char *str)
{
    struct my_str_graffiti *msg = my_malloc_type(struct my_str_graffiti);
    msg->board = my_str_dump(str);
    msg->parts = 1;
    msg->begin = msg->board;
    msg->now = msg->begin;
    msg->end = msg->begin + strlen(msg->begin) + 1;
    return msg;
}

/* Free all resources malloced for string-graffiti(@msg) */
inline void my_str_graffiti_destroy(struct my_str_graffiti *msg)
{
    free(msg->begin);
    free(msg);
}

/* Cut the string given(@str) in many parts according to any char in @delim,
 *  return a string-graffiti to save the cutted string */
inline struct my_str_graffiti *my_str_tok_init(char *str, const char *delim)
{
    char *now;
    struct my_str_graffiti *msg = my_str_graffiti_init(str);
    now = msg->begin;
    while ((now = strpbrk(now, delim)) != NULL)
    {
        *now = '\0';       
        now += 1;
        (msg->parts)++;
    }
    return msg;
}

/* Get Next-part of the cutted string from string-graffiti(@msg) */
inline char *my_str_tok_get(struct my_str_graffiti *msg)
{
    if (msg->now == NULL)
        return NULL;
    char *ret_str = msg->now;
    msg->now += strlen(msg->now) + 1;
    if (msg->now >= msg->end)
        msg->now = NULL;
    return ret_str;
}

/* Get that there are how many parts in cutted string */
inline int my_str_tok_get_parts(struct my_str_graffiti *msg)
{
    return msg->parts;
}

/* Write formated time string to @buffer with offset @off 
 *
 * @stime describe the seconds since "1900-01-01 00:00:00"
 * @mtime describe the mili-seconds left in [0, 999]
 * @return the new offset excluding '\0', MAKE SURE to add '\0' if this is the
 *  last step
 */
inline int my_write_mtime(char *buffer, int off, long stime, int mtime)
{
    char *start = buffer + off;
    strftime(start, SEC_TIME_STR_SIZE, "%F %T", localtime(&stime));
    // overwites char '\0'
    sprintf(start + SEC_TIME_STR_LENGTH, ".%03d", mtime);
    return off + MILISEC_TIME_STR_LENGTH;
}

/* Get the formated time string 
 *
 * @stime describe the seconds since "1900-01-01 00:00:00"
 * @mtime describe the mili-seconds left in [0, 999]
 * @return malloced time string, MAKE SURE to free it when do not need it
 */
inline char *my_get_str_from_mtime(long stime, int mtime)
{
    assert(mtime >= 0 && mtime <= 999);
    char *timestr = my_malloc_ntype(MILISEC_TIME_STR_SIZE, char);
    int writes = my_write_mtime(timestr, 0, stime, mtime);
    assert(writes == MILISEC_TIME_STR_LENGTH);
    timestr[MILISEC_TIME_STR_LENGTH] = '\0';
    return timestr;
}

/* Get long from time string in format "yyyy-mm-dd hh:mm:ss?", '?' is not num */
inline long my_get_long_from_timestr(char* timestr)
{
    struct tm t;
    t.tm_year = atoi(timestr) - 1900;
    t.tm_mon = atoi(timestr + 5) - 1;
    t.tm_mday = atoi(timestr + 8);
    t.tm_hour = atoi(timestr + 11);
    t.tm_min  = atoi(timestr + 14);
    t.tm_sec  = atoi(timestr + 17);
    t.tm_isdst = 0;
    time_t result = mktime(&t);
    return result;
}

/*
 * function : 根据两个指针之间的内容，生成新的字符串，新申请的空间，需要free
 * input    : 两个字符指针，start,end;start指向需要内容的第一个字符，end指向末尾字符的后边一个空间
 * output   : 返回新生成的字符串，malloc了空间，需要free
 */
inline char *createstr(char *start, char *end)
{
    int len = end - start ;
    char *result  = (char *)my_malloc(len + 1);
    memcpy(result, start, len);
    *(char *)(result + len) = '\0';
    return result;
}

/*
 * function : 根据字符串，生成新的字符串，新申请的空间，需要free
 * input    : 字符串指针 str
 * output   : 返回新生成的字符串，malloc了空间，需要free
 */
inline char *my_str_dump(char *str)
{
    size_t len = strlen(str);
    char *result = (char *)my_malloc(len + 1);
    memcpy(result, str, len);
    *(result + len) = '\0';
    return result;
}

/* 
 * function : 查找第一个非空格字符的位置
 * input    : 用于查找的字符串
 * output   : 第一个非空格字符的位置，若全为空格字符则返回NULL
 */

inline char *my_str_first_not_space(char *str)
{
    if (str == NULL)
        return NULL;
    char ch = *str;
    while (ch != '\0')
    {
        if (isspace(ch))
        {
            str++;
            ch = *str;
        }
        else
            return str;
    }
    return NULL;
}

/* 
 * function : 查找第一个空格字符的位置
 * input    : 用于查找的字符串
 * output   : 第一个空格字符的位置，若全为非空格字符则返回NULL
 */
inline char *my_str_first_space(char *str)
{
    if (str == NULL)
        return NULL;
    char ch = *str;
    while (ch != '\0')
    {
        if (! isspace(ch))
        {
            str++;
            ch = *str;
        }
        else
            return str;
    }
    return NULL;
}

/* 
 * function : 截取一个字符串的子串，转化为int
 * input    : char *ori 原始字符串，int from 字串起始位置，int to 字串结束位置
 * output   : 对应的整数
 */
inline int getint(char *ori, int from , int to)
{   
    int n = to - from + 1;
    ori += from;
    char* temp = (char*)my_malloc(n + 1);
    memcpy(temp, ori, n);
    *(temp + n) = '\0';
    int result = atoi(temp);
    free(temp);
    return result;
}

/* 
 * funciton : 获得当前时间 
 * input    : 无
 * output   : 字符串返回
 */
inline char *getnowtime()
{      
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    char *s = asctime(timeinfo);
    int len = strlen(s);
    char *t = (char *)my_malloc(len + 1);
    memcpy(t, s, len);
    *(t + len) = '\0';
    return t;
}

/* 
 * funciton : 获得当前时间 
 * input    : 无
 * output   : 字符串返回
 */
inline time_t getnowltime()
{      
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    return rawtime;
}

/* 
 * function : 拼接两个字符串
 * input    : 需要拼接的两个字符串，按参数位置拼接
 * output   ：拼好的新申请空间存放的字符串
 */
inline char *
mystrcat(char *s1, char *s2){
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    char *s = (char *)my_malloc(len1 + len2 + 1);
    memcpy(s, s1, len1);
    memcpy(s + len1, s2, len2);
    *(s + len1 + len2)= '\0';
    return s;
}

/* 
 * function : 拼接多个字符串
 * input    : 拼接字符串的个数(n_values)，需要拼接的多个字符串，按参数位置拼接
 * output   : 拼好的新申请空间存放的字符串
 */
inline char *my_nstr_cat(size_t n_values, ...)
{
    va_list var_arg;
    size_t size_sum = 0;
    int i;
    // manipulate var_arg
    va_start(var_arg, n_values);
    char *sarray[n_values];
    size_t lens[n_values];
    for (i = 0; i < n_values; i++)
    {
        sarray[i] = va_arg(var_arg, char *);
        if (sarray[i] != NULL)
        {
            lens[i] = strlen(sarray[i]);
            size_sum += lens[i];
        }
        else 
            lens[i] = 0;
    }
    va_end(var_arg);
    // generate result
    char *s = (char *)my_malloc(size_sum + 1);
    char *p = s;
    for (i = 0; i < n_values; i++)
    {
        if (lens[i] > 0)
        {
            memcpy(p, sarray[i], lens[i]);
            p += lens[i];
        }
    }
    *(s + size_sum) = '\0';
    return s;
}

/* 
 * function : 得到整数的位数
 * input    : 要计算的数字
 * output   : 得到的位数
 */
inline int getdigitnum(int i){
    if (i == 0)
        return 1;
    int counter = 0;
    while (i != 0){
        i = i/10;
        counter++;
    }
    return counter;
}

/* 
 * function : 整数转化为字符串
 * input    : 需要转化的整数
 * output   : 结果字符串
 */
inline char *myitoa(int i){
    int len = getdigitnum(i);
    char *s = (char *)my_malloc(len + 1);
    bzero(s, len + 1);
    sprintf(s, "%d", i);
    return s;
}

/*
 * function : 将两个内存块合并
 * input    : 第一个内存块，第一个内存块的大小，第二个内存块，第二个内存块的大小
 * output   : 合并的内存，第一个旧内存块的指针指向这块，然后返回
 */
inline void *memcat(void *block1, int size1, void *block2, int size2)
{
    void *temp = block1;
    block1 = my_malloc(size1 + size2);
    if (block1 == NULL)
        return NULL;
    memcpy(block1, temp, size1);
    memcpy(block1 + size1, block2, size2);
    free(temp);
    return block1;
}

inline int 
str_endwith(char *str, char c)
{
    if (str == NULL)
        return -1;
    char *temp = rindex(str, c);
    if (temp - str + 1 != strlen(str))
        return 0;
    return 1;
}

inline char *
my_pathcat(char *path1, char *path2)
{
    if (path1 == NULL || path2 == NULL)
        return NULL;
    if (str_endwith(path1, '/'))
        return mystrcat(path1, path2);
    return my_nstr_cat(3, path1, "/", path2);
    
}

int is_match(char *s, char *reg)
{
	int status;
	regex_t regt;
	regcomp(&regt,reg,REG_EXTENDED);
	regmatch_t pmatch[1];
	const size_t nmatch=1;
	status=regexec(&regt,s,nmatch,pmatch,0);
	if (status==REG_NOMATCH)
	{
		regfree(&regt);
		return 0;
	}
	else if (status==0)
	{
		int length=pmatch[0].rm_eo-pmatch[0].rm_so;
		int slength=strlen(s);
		regfree(&regt);
		if (length==slength)
			return 1;
		else
			return 0;
	}
}

int hashcode_s(void *a)
{
	char *s=(char *)a;
	int h=0;
	int i=0,count=0,addened=1;
	while (*s!='\0')
	{
		addened=*s;
		for (i=0;i<count;i++)
			addened=(addened<<5)-addened;
		h+=addened;
		count++;
		s++;
	}
	return h;
}
