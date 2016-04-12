/*
 * =============================================================================
 *
 *       Filename:  config.c
 *
 *    Description:  implementation of logic for configs
 *        Version:  1.0
 *       Compiler:  gcc
 *
 *         Author:  quanzhao
 *   Organization:  tigerknows 
 *
 * =============================================================================
 */

#include "../include/config.h"

/***************************** INTERNAL FUNCTIONS *****************************/

static char *
config_get_para(struct config *cnf, char *section, char *name);

/******************************** SOUCE CODES *********************************/

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
struct config *config_load(const char *filename)
{
    struct config *cnf = my_malloc_type(struct config);
    cnf->sections = 0;
    cnf->names = 0;
    cnf->keyvalue = hmap_create_dt(CONFIG_SIZE, NULL, NULL, STRING_KEY);
    char line[CONFIG_LINE_BUFFER_SIZE];
    char section[CONFIG_LINE_BUFFER_SIZE];
    char name[CONFIG_LINE_BUFFER_SIZE];
    char value[CONFIG_LINE_BUFFER_SIZE];
    char tmp[CONFIG_LINE_BUFFER_SIZE];
    section[0] = '\0';
    char *genkey, *genvalue, *avalstr;
    int ret;
    // read file
    FILE *inputfile = fopen(filename, "r");
    if (inputfile == NULL)
        my_error_exit("Open file '%s' FAILED!", filename);
    while (fgets(line, CONFIG_LINE_BUFFER_SIZE, inputfile) != NULL)
    {
        // skip the spaces in front part of a line
        avalstr = my_str_first_not_space(line);
        if (avalstr == NULL)
            avalstr = line;
        // try to read a line of key-value pair
        ret = sscanf(avalstr, "%[^ =#]%[ =]%[^\n#]%*[^\n]", name, tmp, value);  
        if (ret == 3 && strchr(tmp, '=') != NULL)
        {
            // put the key-value pair into hashmap
            genkey = my_nstr_cat(3, section, " ", name);
            genvalue = my_str_dump(value);
            hmap_put(cnf->keyvalue, genkey, genvalue);
            continue;
        }
        // try to read a line of section
        ret = sscanf(avalstr, "[%[^]#]%[^\n]", name, value);
        if (ret == 2 && strchr(value, ']') != NULL)
        {
            // change name of section
            strcpy(section, name);
            continue;
        }
        // skip other cases
    }
    fclose(inputfile);
    return cnf;
}

/* Free all memories malloced for holding configurations */
void config_free(struct config *cnf)
{
    hmap_destroy(cnf->keyvalue);
    free(cnf);
}

/* Get the string of parameter's value */
char *config_get_para(struct config *cnf, char *section, char *name)
{
    char *genkey = my_nstr_cat(3, section, " ", name);  
    char *getvalue = (char *)hmap_get(cnf->keyvalue, genkey);
    free(genkey);
    if (getvalue == NULL)
        my_error_exit("Unknown section or key! section == '%s' & name == '%s'", section, name);
    return getvalue;
}

/* Get string from @cnf with @section and @name, program will exit
 *  and print error message if there are no such parameter.
 */
char *config_get_string(struct config *cnf, char *section, char *name)
{
    char *str;
    char *getstr = config_get_para(cnf, section, name);
    if (getstr != NULL)
    {
        str = my_str_dump(getstr);
        return str;
    }
    else
    {
        my_error_exit("Not a type of string! section == '%s' & name == '%s'", section, name);
    }
}

/* Get int from @cnf with @section and @name, program will exit
 *  and print error message if there are no such parameter.
 */
int config_get_int(struct config *cnf, char *section, char *name)
{
    int result;
    char *getstr = config_get_para(cnf, section, name);
    int ret = sscanf(getstr, "%d", &result);
    if (ret == 1)
        return result;
    else
    {
        my_error_exit("Not a type of int! section == '%s' & name == '%s'", section, name);
    }
}

/* Get ui8 from @cnf with @section and @name, program will exit
 *  and print error message if there are no such parameter.
 */
ui8 config_get_ui8(struct config *cnf, char *section, char *name)
{
    ui8 result;
    char *getstr = config_get_para(cnf, section, name);
    int ret = sscanf(getstr, "%lld", &result);
    if (ret == 1)
        return result;
    else
    {
        my_error_exit("Not a type of ui8! section == '%s' & name == '%s'", section, name);
    }
}

/* Get float from @cnf with @section and @name, program will exit
 *  and print error message if there are no such parameter.
 */
float config_get_float(struct config *cnf, char *section, char *name)
{
    float result;
    char *getstr = config_get_para(cnf, section, name);
    int ret = sscanf(getstr, "%f", &result);
    if (ret == 1)
        return result;
    else
    {
        my_error_exit("Not a type of float! section == '%s' & name == '%s'", section, name);
    }
}


/* Get double from @cnf with @section and @name, program will exit
 *  and print error message if there are no such parameter.
 */
double config_get_double(struct config *cnf, char *section, char *name)
{
    double result;
    char *getstr = config_get_para(cnf, section, name);
    int ret = sscanf(getstr, "%lf", &result);
    if (ret == 1)
        return result;
    else
    {
        my_error_exit("Not a type of double! section == '%s' & name == '%s'", section, name);
    }
}

