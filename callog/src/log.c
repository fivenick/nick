/*
 * =============================================================================
 *
 *       Filename:  log.h
 *
 *    Description:  implementation of logclient
 *        Version:  1.0
 *       Compiler:  gcc
 *
 *         Author:  quanzhao
 *   Organization:  tigerknows 
 *
 * =============================================================================
 */

#include "../include/log.h"

/***************************** INTERNAL FUNCTIONS *****************************/

static void
log_calc_time(struct timeval *nt);

static int 
log_get_levelnumber(enum LOG_LEVEL lv);

static int
log_write_prefix(enum LOG_LEVEL lv, char *prefix, struct timeval *nt);

/****************************** STATIC DATA PART ******************************/

/* Used in formated logs */
static const char LOG_LEVELS[8][6] = {"RECUR", "FATAL", "ERROR", "CRTCL", "WARN ", "MESSA", "INFO ", "DEBUG"};

/* Static pointer to log's handler for now */ 
static struct loghandler *nlhd = NULL;

/******************************** SOUCE CODES *********************************/

/* Init a normal loghandler, logs will be print to stderr */
void log_init_offline(enum LOG_LEVEL mask)
{
    if (nlhd != NULL)
        my_error_exit("Log(offline) has been initialized twice!");
    nlhd = my_malloc_type(struct loghandler);
    nlhd->context = NULL;
    nlhd->sender = NULL;
    nlhd->protocal = NULL;
    nlhd->ip = NULL;
    nlhd->port = NULL;
    nlhd->endpoint = NULL;
    nlhd->sndHWM = 0;
    nlhd->mode = 0;
    nlhd->mask = mask;
}

/* Init a logclient connect to logserver, logserver must be initialized */
void log_init_client(char *configfile)
{
    if (nlhd == NULL)
        //log_init_offline(LOG_LEVEL_DEBUG);
        log_init_offline(LOG_LEVEL_MASK);
    if (nlhd->context != NULL)
        my_error_exit("Log(online) has been initialized twice!");
    struct config *cnf = config_load(configfile);
    nlhd->mask = config_get_int(cnf, "logclient", "mask");
    nlhd->mode = config_get_int(cnf, "logclient", "mode");
    // offline mode
    if (nlhd->mode == 0)
    {
        config_free(cnf);
        return;
    }
    // online mode
    nlhd->protocal = config_get_string(cnf, "logcommon", "protocal");
    nlhd->ip = config_get_string(cnf, "logcommon", "ip");
    nlhd->port = config_get_string(cnf, "logcommon", "port");
    if (strcmp(nlhd->protocal, LOG_PROTOCAL_TCP) == 0)
        nlhd->endpoint = my_nstr_cat(5, nlhd->protocal, "://", nlhd->ip, ":", nlhd->port);
    else if (strcmp(nlhd->protocal, LOG_PROTOCAL_IPC) == 0)
        nlhd->endpoint = my_nstr_cat(3, nlhd->protocal, ":///tmp/logserver_", nlhd->port);
    else
        my_error_exit("Unknown protocal: %s", nlhd->protocal);
    nlhd->context = zmq_ctx_new();
    // TODO
    //zmq_ctx_set(nlhd->context, ZMQ_MAX_SOCKETS, 10);
    //zmq_ctx_set(nlhd->context, ZMQ_IO_THREADS, 10);
    nlhd->sender = zmq_socket(nlhd->context, ZMQ_PUSH);
    int ret;
    nlhd->sndHWM = config_get_int(cnf, "logclient", "sndHWM");
    ret = zmq_setsockopt(nlhd->sender, ZMQ_SNDHWM, &(nlhd->sndHWM), sizeof(nlhd->sndHWM));
    assert(ret == 0);
    ret = zmq_connect(nlhd->sender, nlhd->endpoint);
    if (ret != 0)
        my_error_exit("Logsercer has NOT been initialized! Connect FAIL!");
    config_free(cnf);
}

/* Free all resources and memories malloced for loghandler */
void log_free()
{
    if (nlhd == NULL)
        return;
    if (nlhd->context != NULL)
    {
        zmq_close(nlhd->sender);
        zmq_ctx_destroy(nlhd->context);
    }
    free(nlhd->protocal);
    free(nlhd->ip);
    free(nlhd->port);
    free(nlhd->endpoint);
    free(nlhd);
}

/* Use system call to update time information */
inline void log_calc_time(struct timeval *nt)
{
    gettimeofday(nt, NULL);
    // utimenow = (long long int)nt.tv_sec * UNISECS_SEC + nt.tv_usec;
}

/* Get the number of a log level enum */
int log_get_levelnumber(enum LOG_LEVEL lv)
{
    switch (lv & LOG_LEVEL_MASK)
    {
        case LOG_FLAG_RECURSION:
            return 0;   
        case LOG_FLAG_FATAL:
            return 1;
        case LOG_LEVEL_ERROR:
            return 2;
        case LOG_LEVEL_CRITICAL:
            return 3;
        case LOG_LEVEL_WARNING:
            return 4;
        case LOG_LEVEL_MESSAGE:
            return 5;
        case LOG_LEVEL_INFO:
            return 6;
        case LOG_LEVEL_DEBUG:
            return 7;
    }
    my_error_exit("Unknown LOG_LEVEL!");
}

/* Write strings of log's prefix to buffer "prefix" */
int log_write_prefix(enum LOG_LEVEL lv, char *prefix, struct timeval *nt)
{
    int writes = 0;
    int lvnum = log_get_levelnumber(lv);
    // have called "log_calc_time()" before
    writes = my_write_mtime(prefix, 0, nt->tv_sec, nt->tv_usec / 1000);
    writes += sprintf(prefix + writes, " [%s] ", LOG_LEVELS[lvnum]);
    return writes;
}

/* Use it as "printf" */
void log_message(enum LOG_LEVEL lv, char *format, ...)
{
    // logclient hasn't been initialized, switch to DEBUG mode
    if (nlhd == NULL)
    {
        log_init_offline(LOG_LEVEL_MASK);
    }
    // check whether need to log
    if (lv & nlhd->mask)
    {
        int writes, tmp;
        struct timeval nowtime;
        char onelog[LOG_MAX_SIZE];
        // get and update static variables
        log_calc_time(&nowtime);
        // writes the formated log to local buffer "onelog"
        writes = log_write_prefix(lv, onelog, &nowtime);
        assert(writes >= 0);
        va_list var_arg;
        va_start(var_arg, format);
        int maxvarsize = LOG_MAX_SIZE - writes - 1;
        tmp = vsnprintf(onelog + writes, maxvarsize, format, var_arg);
        va_end(var_arg);
        if (tmp < 0)
        {
            fprintf(stderr, "Logclient error! Can NOT format message!\n");
            fflush(stderr);
            return;
        }
        if (tmp >= maxvarsize)
        {
            fprintf(stderr, "Too long size of log, greater than %d(bytes)\n", LOG_MAX_SIZE);
            fflush(stderr);
            tmp = maxvarsize;
        }
        writes += tmp;
        onelog[writes] = '\n';
        writes++;
        // print to stderr
        if (nlhd->context == NULL)
        {
            fwrite(onelog, writes, 1, stderr);
        }
        // send message to logserver
        else
        {
            zmq_msg_t smsg;
            zmq_msg_init_size(&smsg, writes);
            memcpy(zmq_msg_data(&smsg), onelog, writes);
            // print to stderr if sndHWM can not handle the current message
            int ret = zmq_msg_send(&smsg, nlhd->sender, ZMQ_DONTWAIT);
            if (ret == -1)
                fwrite(onelog, writes, 1, stderr);
            zmq_msg_close(&smsg);
        }
    }
}

