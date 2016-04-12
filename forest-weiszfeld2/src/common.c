
#include "../include/common.h"

/* The normal destroyer for free the memories malloced */
inline void common_destroyer_norm(void *pointer)
{
    free(pointer);
}

/* Fake destroyer(will not do anything) */
inline void common_destroyer_fake(void *pointer)
{
    return;
}

/* Action when received SIGSEGV */
void common_ouch_SEGV(int sig)
{
    // trace function calls
    char *timestr = my_get_timestr();
    fprintf(stderr, "At time: %s\n", timestr);
    free(timestr);
    fprintf(stderr, "Caught SIGSEGV(%d): Segmentfault\n", sig);
    my_backtrace();
    fflush(stdout);
    fflush(stderr);
    // write a log
    log_error("Segmentfault");
    // free resources
    DESTROY();
    exit(-1);
}

