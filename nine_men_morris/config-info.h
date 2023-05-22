#ifndef _CONFIGINFO_H
#define _CONFIGINFO_H
/**
 * config_item is used to receive the specified configuration file in "parameterName = parameterWert"
 */

char key[50], eq[4];
char hName[32], pNumber[32], gKindName[32];
typedef struct {
    char *hostName;
    int portNumber;
    char *gameKindName;
}configInfo;


#endif