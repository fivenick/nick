/*
 * =============================================================================
 *
 *       Filename:  config.h
 *
 *    Description:  defination of logic for configs
 *        Version:  1.0
 *       Compiler:  gcc
 *
 *         Author:  quanzhao
 *   Organization:  tigerknows 
 *
 * =============================================================================
 */

#include "common.h"

#ifndef __CONFIG_H__
#define __CONFIG_H__

/********************************* DEFINATIONS ********************************/

#define CONFIG_SIZE (1024 * 4)
#define CONFIG_LINE_BUFFER_SIZE (1024)

struct config
{
    struct hmap *keyvalue;
    size_t sections;
    size_t names;
};

/****************************** EXTERN FUNCTIONS ******************************/

/* Load and hold configurations in "filename".
 *
 * MAKE SURE to call function "config_free" to free all memories malloced. The
 *  format of the config-file is as following:
 *
 * # there can be many sections, different section can have a same para
 * # @section name must be in a pair of "[" and "]"
 * [@section name]
 * # no space in @para_name, one line for one para
 * @para_name = @para_value
 * ...
 */
extern struct config *
config_load(const char *filename);

/* Free all memories malloced for holding configurations */
extern void 
config_free(struct config *cnf);

/* Get the parameter from "cnf" with "section" and "name", program will exit
 *  and print error message if there are no such parameter.
 */
extern char *
config_get_string(struct config *cnf, char *section, char *name);

extern int 
config_get_int(struct config *cnf, char *section, char *name);

extern ui8 
config_get_ui8(struct config *cnf, char *section, char *name);

extern float
config_get_float(struct config *cnf, char *section, char *name);

extern double 
config_get_double(struct config *cnf, char *section, char *name);

#endif /* __CONFIG_H__ */

