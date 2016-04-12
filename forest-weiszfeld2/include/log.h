/*
 * =============================================================================
 *
 *       Filename:  log.h
 *
 *    Description:  defination of logclient
 *        Version:  1.0
 *       Compiler:  gcc
 *
 *         Author:  quanzhao
 *   Organization:  tigerknows 
 *
 * =============================================================================
 */

#include "common.h"

#ifndef __LOG_H__
#define __LOG_H__

/********************************* DEFINATIONS ********************************/

/* ATTENSION: Suggested to use MACROs but not extern functions!
 * 
 * Use "INIT()" and "DESTROY()" defined in "common.h" in main function to init
 *  and free memories used by this module.
 */

/* Constant string of protocal */
#define LOG_PROTOCAL_TCP "tcp"
#define LOG_PROTOCAL_IPC "ipc"

/* Maximal size of one log */
#define LOG_MAX_SIZE (1024 * 1024)

/* Use following macros as "printf" */
#define log_error(...) log_message(LOG_LEVEL_ERROR, __VA_ARGS__)
#define log_critical(...) log_message(LOG_LEVEL_CRITICAL, __VA_ARGS__)
#define log_warn(...) log_message(LOG_LEVEL_WARNING, __VA_ARGS__)
#define log_msg(...) log_message(LOG_LEVEL_MESSAGE, __VA_ARGS__)
#define log_info(...) log_message(LOG_LEVEL_INFO, __VA_ARGS__)
/* Ignore all log_debug if not in debuging */
#ifndef NDEBUG
#define log_debug(...) log_message(LOG_LEVEL_DEBUG, __VA_ARGS__)
#else
#define log_debug(...) do { } while(false) 
#endif

/* Log levels and flags */
enum LOG_LEVEL
{
    // log flags
    LOG_FLAG_RECURSION      = 1 << 0,   // reserved
    LOG_FLAG_FATAL          = 1 << 1,   // reserved
    // log levels
    LOG_LEVEL_ERROR         = 1 << 2,
    LOG_LEVEL_CRITICAL      = 1 << 3,
    LOG_LEVEL_WARNING       = 1 << 4,
    LOG_LEVEL_MESSAGE       = 1 << 5,
    LOG_LEVEL_INFO          = 1 << 6,
    LOG_LEVEL_DEBUG         = 1 << 7,
    // mask
    LOG_LEVEL_MASK          = ~(LOG_FLAG_RECURSION | LOG_FLAG_FATAL)
};

/* Hold data for a log-handler */
struct loghandler
{
    void *context;          // 0mq's context
    void *sender;           // 0mq's socket
    char *protocal;         // protocal of 0mq's socket, tcp|ipc
    char *ip;               // ip address of the logserver
    char *port;             // port number when connection to this server
    char *endpoint;         // 0mq's endpoint
    int sndHWM;             // High Water Mark of sending queue
    int mode;               // 0: offline, 1: online
    enum LOG_LEVEL mask;    // log level
};

/****************************** EXTERN FUNCTIONS ******************************/

/* Init a normal loghandler, logs will be print to stderr */
extern void
log_init_offline(enum LOG_LEVEL lv);

/* Init a logclient connect to logserver, logserver must be initialized */
extern void
log_init_client(char *configfile);

/* Free all resources and memories malloced for loghandler */
extern void
log_free();

/* Use it as "printf" */
extern void
log_message(enum LOG_LEVEL lv, char *format, ...);

#endif /* __LOG_H__ */

