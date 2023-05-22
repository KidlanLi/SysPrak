#ifndef _CONFIG_H
#define _CONFIG_H
# include "config-info.h"

/**
 * item stores variable names and values in the format parameterName = parameterValue*/
typedef struct {
    char *name;
    char *value;
} config_item;
/**
 * When processing the config file, each line of "parameterName = parameterValue" will be stored in a config_item
 * the rows in a file are linked into a config_item chain*/


configInfo config(char *config_path);
void configPrint(configInfo conf);
void configInfoClear(configInfo *item);

#endif //_CONFIG_H
