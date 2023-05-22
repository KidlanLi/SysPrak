#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h> // for close
#include "config-info.h"

int connectServer(configInfo *conf) {


    // create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock < 0) {
        printf("Unable to create socket.\n");
        return -1;
    }
    struct sockaddr_in address; // store port, IP address
    //const char *HOSTNAME = "sysprak.priv.lab.nm.ifi.lmu.de";

    struct hostent *host; // store host info
    host = gethostbyname(conf->hostName); // gethostbyname() returns a pointer to hostent structure
    memcpy(&address.sin_addr, host->h_addr_list[0], host->h_length); // copy IP address to sin_addr

    // initialize server’s address information in sockaddr_in
    address.sin_family = AF_INET;
    address.sin_port = htons(conf->portNumber);

    // send connection request to server
    if (connect (sock, (struct sockaddr*) &address, sizeof(address)) < 0){
        printf("Unable to connect.\n");
        return -1;
    }
    

    
    return sock;
}

void closeSock(int sock) {
    // close the socket
    close(sock);
}

