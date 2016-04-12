/*
 * =============================================================================
 *
 *       Filename:  logserver.c
 *
 *    Description:  implementation of logserver
 *        Version:  1.0
 *       Compiler:  gcc
 *
 *         Author:  quanzhao
 *   Organization:  tigerknows 
 *
 * =============================================================================
 */

#include "../include/logserver.h"
#include <sys/time.h>

/***************************** INTERNAL FUNCTIONS *****************************/

static void 
logserver_getopt(int argc, char **argv);

static void
logserver_savepid();

static char *
logserver_new_suffix();

static void 
logserver_init();

static void
logserver_destroy();

static void
logserver_usage();

static void 
logserver_status();

static void
logserver_try_close();

static void 
logserver_ouch_INT(int sig);

static void
logserver_ouch_ALRM(int sig);

static void 
logserver_ouch_USR1(int sig);

static void 
logserver_ouch_USR2(int sig);

static void 
logserver_serve();

/****************************** STATIC DATA PART ******************************/

/* Static pointer for saving information of the logserver */
static struct logserver *lsv = NULL;

/* Static timer */
static struct itimerval mytimer;

/* Hold the time-data from system call */
static struct timeval nowtime;
// static long long int utimenow;

/******************************** SOUCE CODES *********************************/

/* Get the options */
void logserver_getopt(int argc, char **argv)
{
    int opt;
    lsv->configfile = NULL;
    lsv->pidfile = NULL;
    lsv->verbose = false;
    struct option longopts[] =  
    {   
        {"config", 1, NULL, 'c'},
        {"mark", 1, NULL, 'm'},
        {"verbose", 0, NULL, 'v'},
        {0, 0, 0, 0}
    }; 
    while ((opt = getopt_long(argc, argv, ":c:m:v", longopts, NULL)) != -1)
    {
        switch (opt)
        {
            case 'c':
                lsv->configfile = my_str_dump(optarg);
                break;
            case 'm':
                lsv->pidfile = my_str_dump(optarg);
                break;
            case 'v':
                lsv->verbose = true;
                break;
            case '?':
                logserver_usage();
                my_error_exit("Unknown option: %c\n", optopt);
                break;
        }
    }
    // use the default parameter as configfile
    if (lsv->configfile == NULL && optind < argc)
        lsv->configfile = my_str_dump(argv[optind]);
}

/* Save pid to file "pidfile" or use LOGSERVER_DFT_PIDFILE as default "pidfile" */
void logserver_savepid()
{
    FILE *pidstream = fopen(lsv->pidfile, "w");
    if (pidstream == NULL)
        my_error_exit("CANNOT open file '%s'", lsv->pidfile);
    fprintf(pidstream, "%d", getpid());
    fclose(pidstream);
}

/* Use system call to update time information */
inline void logserver_calc_time()
{
    gettimeofday(&nowtime, NULL);
    //utimenow = (long long int)nowtime.tv_sec * UNISECS_SEC + nowtime.tv_usec;
}

/* Generate new suffix for time now, MAKE SURE to free (char *)result */
inline char *logserver_new_suffix()
{
    logserver_calc_time();
    char suffix[LOGSERVER_MAX_SUFFIX_LENGTH];
    // have called "log_calc_time()" before
    strftime(suffix, LOGSERVER_MAX_SUFFIX_LENGTH, lsv->suffixFMT, localtime(&(nowtime.tv_sec)));
    char *result = my_str_dump(suffix);
    return result;
}

/* Initialize the server of log */
void logserver_init()
{
    // load configfile
    if (lsv->configfile == NULL)
    {
        logserver_usage();
        my_error_exit("Configfile must be given!");
    }
    struct config *cnf = config_load(lsv->configfile);
    // check pidfile 
    if (lsv->pidfile == NULL || access(lsv->pidfile, W_OK) == -1)
    {
        logserver_usage();
        my_error_exit("pidfile must be given!");
    }
    // try to get logfile
    lsv->logfile = config_get_string(cnf, "logserver", "file");
    lsv->suffixFMT = config_get_string(cnf, "logserver", "suffixFMT");
    lsv->suffixOLD = logserver_new_suffix(); 
    lsv->rffc = config_get_int(cnf, "logserver", "rffc");
    lsv->isstd = true;
    if (strcmp(lsv->logfile, LOGSERVER_STDOUT) == 0)
        lsv->stream = stdout;
    else if (strcmp(lsv->logfile, LOGSERVER_STDERROR) == 0)
        lsv->stream = stderr;
    else
    {
        lsv->stream = fopen(lsv->logfile, "a");
        if (lsv->stream == NULL)
            my_error_exit("CANNOT open file '%s'", lsv->logfile);
        lsv->isstd = false;
    }
    // init logserver
    lsv->protocal = config_get_string(cnf, "logcommon", "protocal");
    lsv->port = config_get_string(cnf, "logcommon", "port");
    if (strcmp(lsv->protocal, LOG_PROTOCAL_TCP) == 0)
        lsv->endpoint = my_nstr_cat(3, lsv->protocal, "://*:", lsv->port);
    else if (strcmp(lsv->protocal, LOG_PROTOCAL_IPC) == 0)
        lsv->endpoint = my_nstr_cat(3, lsv->protocal, ":///tmp/logserver_", lsv->port);
    else
        my_error_exit("Unknown protocal: %s", lsv->protocal);
    int ret;
    lsv->context = zmq_ctx_new();
    lsv->receiver = zmq_socket(lsv->context, ZMQ_PULL);
    // set options of socket
    lsv->rcvHWM = config_get_int(cnf, "logserver", "rcvHWM");
    ret = zmq_setsockopt(lsv->receiver, ZMQ_RCVHWM, &(lsv->rcvHWM), sizeof(lsv->rcvHWM));
    assert(ret == 0);
    // bind socket
    ret = zmq_bind(lsv->receiver, lsv->endpoint);
    if (ret == -1)
    {
        perror("0mq bind socket");
        my_error_exit("Logserver init fail!\nEndpoint: %s", lsv->endpoint);
    }
    lsv->serve = true;
    if (lsv->verbose)
        fprintf(stderr, "Logserver has been initialized!\n");
    // free configfile
    config_free(cnf);
}

/* Shut down all resources used and free all memories malloced */
void logserver_destroy()
{
    if (! lsv->isstd)
        fclose(lsv->stream);
    free(lsv->configfile);
    free(lsv->logfile);
    free(lsv->pidfile);
    free(lsv->protocal);
    free(lsv->port);
    free(lsv->endpoint);
    free(lsv->suffixFMT);
    free(lsv->suffixOLD);
    logserver_try_close();
    int ret = zmq_ctx_destroy(lsv->context);
    if (ret == -1)
        perror("0mq context destroy");
    if (lsv->verbose)
        fprintf(stderr, "Logserver has been shutdown!\n");
    free(lsv);
    log_free();
}

/* Print status of logserver to stderr */
void logserver_status()
{
    fprintf(stderr, "-------- Logserver status --------\n");
    fprintf(stderr, "Log file -> %s\n", lsv->logfile);
    fprintf(stderr, "Refresh frequency -> %d(seconds)\n", lsv->rffc);
    fprintf(stderr, "Bind address -> %s\n", lsv->endpoint);
    fprintf(stderr, "Receiving High Water Mark -> %d\n", lsv->rcvHWM);
    fprintf(stderr, "Config file -> %s\n", lsv->configfile);
    fprintf(stderr, "PID saving file -> %s\n", lsv->pidfile);
    if (lsv->serve)
        fprintf(stderr, "Status -> ON\n");
    else
        fprintf(stderr, "Status -> OFF\n");
    if (lsv->verbose)
        fprintf(stderr, "Verbose Mode!\n");
    fprintf(stderr, "----------------------------------\n");
}

/* Try to close socket communications and refuse any connection from now on */
inline void logserver_try_close()
{
    if (lsv->serve)
    {
        int ret = zmq_close(lsv->receiver); 
        if (ret == -1)
            perror("0mq socket close");
        else
            lsv->serve = false;
    }
}

/* Action when received SIGINT, close socket */
void logserver_ouch_INT(int sig)
{
    // case when parent-process exit directlly
    if (lsv == NULL)
        return;
    if (lsv->verbose)
    {
        fprintf(stderr, "Receiced signal %d\n", sig);
        fprintf(stderr, "Try to close socket of logserver...\n");
    }
    logserver_try_close();
    if (lsv->verbose)
    {
        if (! lsv->serve)
            fprintf(stderr, "Socket of logserver closed OK!\n");
        else
            fprintf(stderr, "Error: Socket of logserver CANNOT be closed!\n");
    }
}

/* Action when received SIGALRM, refresh log-file */
void logserver_ouch_ALRM(int sig)
{ 
    if (! lsv->isstd)
    {
        char *suffixNEW = logserver_new_suffix(); 
        // do not need a new log-file
        if (strcmp(lsv->suffixOLD, suffixNEW) == 0)
        {
            free(suffixNEW);
            fflush(lsv->stream);
            return;
        } 
        // save old log-file and open a new one
        else
        {
            fclose(lsv->stream);
            char *newfilename = my_nstr_cat(2, lsv->logfile, lsv->suffixOLD);
            // rename old log-file
            rename(lsv->logfile, newfilename);
            free(newfilename);
            free(lsv->suffixOLD);
            lsv->suffixOLD = suffixNEW;
            lsv->stream = fopen(lsv->logfile, "a");
        }
    }
}

/* Action when received SIGUSR1, flush all logs to real file */
void logserver_ouch_USR1(int sig)
{
    if (lsv->verbose)   
        fprintf(stderr, "Receiced signal %d\n", sig);
    fflush(lsv->stream);
}

/* Action when received SIGUSR2, print status of logserver to stderr */
void logserver_ouch_USR2(int sig)
{
    if (lsv->verbose)   
        fprintf(stderr, "Receiced signal %d\n", sig);
    logserver_status();
}

/* Loop forever to receive sockets communications */
void logserver_serve()
{
    if (lsv->verbose)
    {
        fprintf(stderr, "Logserver is ON SERVE NOW!\n");
        logserver_status();
    }
    // set alarm clock for refreshing
    mytimer.it_value.tv_sec = 1;
    mytimer.it_interval.tv_sec = lsv->rffc;
    setitimer(ITIMER_REAL, &mytimer, NULL);
    // report to parent-process that logserver is right on serve
    kill(getppid(), SIGINT);
    // serve loop
    char *msgstr;
    int msgsize;
    while (true)
    {
        zmq_msg_t request;
        zmq_msg_init(&request);
        zmq_msg_recv(&request, lsv->receiver, 0);
        // break to close bind socket if lsv->serve == false
        if (! lsv->serve)
        {
            zmq_msg_close(&request);
            break;
        }
        // write log
        msgsize = zmq_msg_size(&request);
        if (msgsize > 0)
        {
            fwrite(zmq_msg_data(&request), msgsize, 1, lsv->stream);
            if (ferror(lsv->stream) && errno == EINTR)
            {
                fprintf(stderr, "\nEINTR Error\n");
                clearerr(lsv->stream);
                fflush(lsv->stream);
            }
        }
        zmq_msg_close(&request);
    }
    logserver_destroy();
}

/* Print usage of the logserver */
void logserver_usage()
{
    fprintf(stderr, "Usage: ./build/logserver\n");
    fprintf(stderr, "\t[-c|--config] configfile\n");
    fprintf(stderr, "\t-m|--mark pidfile\n");
    fprintf(stderr, "\t[-v|--verbose]\n");
    fprintf(stderr, "Signals can be sent to this process:\n");
    fprintf(stderr, "\tSIGINT: shut down the server\n");
    fprintf(stderr, "\tSIGUSR1: flush logs to real file\n");
    fprintf(stderr, "\tSIGUSR2: print status of the server to stderr\n");
}

/* Main process */
/*int main(int argc, char *argv[])
{
    // catch signals
    my_catch_sig_exclusive(SIGINT, logserver_ouch_INT);
    my_catch_sig_exclusive(SIGALRM, logserver_ouch_ALRM);
    my_catch_sig_exclusive(SIGUSR1, logserver_ouch_USR1);
    my_catch_sig_exclusive(SIGUSR2, logserver_ouch_USR2);
    // fork a sub-process
    pid_t pid = fork();
    switch (pid)
    {
        case -1:
            perror("Logserver fork");
            my_error_exit("CANNOT fork a sub-process!");
            break;
        case 0:
            // new session and new process-group
            setsid();
            // init the logserver
            assert(lsv == NULL);
            lsv = my_malloc_type(struct logserver);
            logserver_getopt(argc, argv);
            logserver_init();   
            // save pid
            logserver_savepid();
            // start to serve now
            logserver_serve();            
            break;
        default:
            // wait until sub-process init OK and send SIGINT
            pause();
            break;        
    }
    return 0;
}*/
