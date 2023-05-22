#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include "config-info.h"
#include "config.h"


/**
 * As an extension interface for future configs:
 * Create a new config entry with name and value
 * name: paramaterName
 * value: parameterValue*/
config_item *configItemNew(char *name, char *value) {
    config_item *item = malloc(sizeof(config_item));
    if (item == NULL) {
        return NULL;
    }
    memset(item, 0, sizeof(config_item));
    item->name = malloc(strlen(name));
    if (item->name == NULL) {
        free(item);
        return NULL;
    }
    memset(item->name, 0, strlen(name));
    memcpy(item->name, name, strlen(name));
    item->value = malloc(strlen(value));
    if (item->value == NULL) {
        free(item->name);
        free(item);
        return NULL;
    }
    memset(item->value, 0, strlen(value));
    memcpy(item->value, value, strlen(value));
    return item;
}

/**
 * Release the item at the incoming config_item pointer*/
void configItemFree(config_item *item) {
    free(item->name);
    free(item->value);
    free(item);
}

/**
 * Release the item at the incoming configInfo pointer*/
void configInfoClear(configInfo *item) {
    free(item -> hostName);
    free(item -> gameKindName);
}

/**
 * Retrieve the items in the specified config_path*/
configInfo config(char *config_path) {
    FILE *fp;
    // If the pointer passed in is null, the default config is read
    char *str = "client.conf"; // ---
    if (config_path == NULL || strlen(config_path) <= 0 || strcmp(config_path,str)==0) {
        config_path = "client.conf";
    }


    if ((fp = fopen(config_path, "r")) < 0) {
        printf("file is error!\n");
        exit(0);
    }

    char buff[255];
//    char key[50], eq[4];
//    char hName[32], pNumber[32], gKindName[32];
    // Extract the first three lines of the config file and name them in order
    configInfo conf;
    int portNum;

    // Extract the first line in the file, the variable after the equal sign as hostName
    fscanf(fp, "%s%s%s", key, eq, hName);
    (&conf)->hostName = (char *) malloc(31 * sizeof(char));
    strcpy((&conf)->hostName,hName);
//    printf("Find hostName: %s\n", conf.hostName);
    fgets(buff, sizeof(buff), fp);

    // Extract the second line in the file, the variable after the equal sign as portNumber
    fscanf(fp, "%s%s%s", key, eq, pNumber);
    portNum = atoi(pNumber);
    conf.portNumber = portNum;
//    printf("Find portNumber: %d\n", conf.portNumber);
    fgets(buff, sizeof(buff), fp);

    // Extract the third line of the file, the variable after the equal sign as gameKindName
    fscanf(fp, "%s%s%s", key, eq, gKindName);
//    conf.gameKindName = gKindName;
    (&conf)->gameKindName = (char *) malloc(9 * sizeof(char));
    strcpy((&conf)->gameKindName,gKindName);
//    printf("Find gameKindName: %s\n", conf.gameKindName);
    return conf;
}



void configPrint(configInfo conf) {
    printf("Find hostName: %s\n", conf.hostName);
    printf("Find portNumber: %d\n", conf.portNumber);
    printf("Find gameKindName: %s\n", conf.gameKindName);
}