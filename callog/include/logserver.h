/*
 * =============================================================================
 *
 *       Filename:  logserver.h
 *
 *    Description:  defination of logserver
 *        Version:  1.0
 *       Compiler:  gcc
 *
 *         Author:  quanzhao
 *   Organization:  tigerknows 
 *
 * =============================================================================
 */

#include "common.h"

#ifndef __LOGSERVER_H__
#define __LOGSERVER_H__

/********************************* DEFINATIONS ********************************/

/* ATTENTION that the source code got main function so DO NOT include this*/
/* Usage:
 * 1. cd to main directory and run "make"
 * 2. ./build/logserver 
 *      [-c|--config] configfile 
 *      -m|--mark pidfile
 *      [-v|--verbose]
 * Signals can be sent to this process:
 *  1. SIGINT: shut down the server
 *  2. SIGUSR1: flush logs to real file
 *  3. SIGUSR2: print status of the server to stderr
 */

/* Parameter "file" in config */
#define LOGSERVER_STDOUT "STDOUT"
#define LOGSERVER_STDERROR "STDERROR"

/* Max size of suffix string for an old log-file */
#define LOGSERVER_MAX_SUFFIX_LENGTH 32

/* Datas hold for logserver */
struct logserver
{
    void *context;      // 0mq's context
    void *receiver;     // 0mq's socket
    FILE *stream;       // file stream for logs
    char *configfile;   // config file
    char *logfile;      // file for log
    char *pidfile;      // file for saving pid
    char *protocal;     // protocal of 0mq's socket, tcp|ipc
    char *port;         // port number when connection to this server
    char *endpoint;     // 0mq's endpoint
    char *suffixFMT;    // format of suffix string for defining an old log-file
    char *suffixOLD;    // old suffix string
    int rcvHWM;         // High Water Mark of receiving queue
    int rffc;           // refresh frequency(seconds) of the log-file
    bool serve;         // whether the server is on serve now
    bool verbose;       // whether print more information to stderr
    bool isstd;         // log file is stdout, stderr or realfile
};

#endif /* __LOGSERVER_H__ */

