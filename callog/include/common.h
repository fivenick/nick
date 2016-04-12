/* Common header */

#ifndef __COMMON_H__
#define __COMMON_H__

/* Define byte-order of machine */
#define __SMALL_ENDIAN__
//#define __BIG_ENDIAN__

/* Basic support */
#include <stdio.h>
#include <time.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <assert.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>
/* Mutil-thread support */
#include <pthread.h>
#include <semaphore.h>
/* support long options */
#define _GNU_SOURCE
#include <getopt.h>
#include <regex.h>

/* ignore following: 
 * assert(...) 
 * log_debug(...) 
 * struct famhmap.rsize
 * famhmap_fun_check(struct famhmap *fh, void * key)
 */
//#define NDEBUG

#define TRUE 1
#define FALSE 0

#define INT_KEY 'i'

#define STRING_KEY 'c'

#define INT_KEY_LENGTH sizeof(int)

#define STRING_KEY_LENGTH 31

#define READ_BUFFER_SIZE (1024 * 21 * 50)

#define HSIZE 10000000

#define COMMON_WRITE_SIZE (50 * 1024 * 1024)
/* Main directory of all config-files */
#define MAIN_CONF_DIR "/../conf/"
/* Some common config-files */
#define LOG_CONF "log.conf"
#define MACHINE_CONF "machine.conf"
/* Hyphen for separated sections in config-files */
#define COMMON_CNF_SEC_HYPHEN " "
/* Section-name of config-file for defining virtual names of machines */
#define MACHINE_CNF_SEC_VIRTUAL "machine virtual"
#define MACHINE_CNF_HOSTNAME_MAXSIZE 32

//记录和key的结尾，及填充字符
#define keytail '\0'

/* Max int value */
#define INT_MAX (~(unsigned int)0 / 2)

typedef unsigned long long ui8;

/* Max type to save a number(int|long), DO NOT use unsigned */
typedef int numlimit_t;

/* Common struct for bytes */
struct common_xbyte
{
    int bsize;      // size of bytes
    char bdata[];   // data of bytes
};
/* Common struct for bytes with only a size and a pointer */
struct common_xbyte_p
{
    int bsize;      // size of bytes
    char *bdata;    // pointer of bytes
};

/* BE CAREFUL TO CHANGE THE POSITION AND THE SEQUENCE */
/* Basic tools */
#include "myutil.h"
#include "heapsort.h"
/* Advanced tools */
#include "list.h"
#include "hashmap.h"
#include "emblist.h"
#include "famhmap.h"
/* Senior tools */
#include "config.h"
/* Basic Important Common modules */
#include <zmq.h>
#include "log.h"
#include "logserver.h"
#include "t_file.h"

#define common_xbyte_init(bytes)                                               \
    (struct common_xbyte *)my_malloc(sizeof(struct common_xbyte) + (bytes))

/* Change byte-order between binary-file and localhost */
#ifdef __SMALL_ENDIAN__
#define disk_memcpy(dst, src, len)                                             \
    memcpy(dst, src, len)
#elif defined( __BIG_ENDIAN__ )
#define disk_memcpy(dst, src, len)                                             \
    do                                                                         \
    {                                                                          \
        char *hpos, *tpos, *head;                                              \
        for (hpos = (char *)dst, head = (char *)src, tpos = head + len - 1;    \
                tpos >= head; tpos--, hpos++)                                  \
            *hpos = *tpos;                                                     \
    }                                                                          \
    while (false)
#else
#define disk_memcpy(dst, src, len)                                             \
    ERROR_PLEASE_DEFINE_ENDIANS
#endif

/* Common defination of INIT() and DESTROY() */
char *maindir;
#define INIT()                                                                 \
    do                                                                         \
    {                                                                          \
        char *start = argv[0];                                                 \
        char *end = strrchr(start, '/');                                       \
        if (end == NULL)                                                       \
            end = start + strlen(start);                                       \
        maindir = createstr(start, end);                                       \
        char *logconfig = my_nstr_cat(3, maindir, MAIN_CONF_DIR, LOG_CONF);    \
        log_init_client(logconfig);                                            \
        free(logconfig);                                                       \
        my_catch_sig_exclusive(SIGSEGV, common_ouch_SEGV);                     \
    }                                                                          \
    while (false)

#define DESTROY()                                                              \
    do                                                                         \
    {                                                                          \
        free(maindir);                                                         \
        log_free();                                                            \
        fflush(stdout);                                                        \
        fflush(stderr);                                                        \
    }                                                                          \
    while (false)

/* The normal destroyer for free the memories malloced */
extern void
common_destroyer_norm(void *pointer);

/* Fake destroyer(will not do anything) */
extern void
common_destroyer_fake(void *pointer);

/* Signal handling function for receiving SIGSEGV */
extern void
common_ouch_SEGV(int sig);

#endif /* __COMMON_H__ */

