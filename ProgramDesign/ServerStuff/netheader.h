#ifndef SERVER
#define SERVER

#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/ioctl.h>


#include "header.h"

#define BS 1024

typedef struct addrinfo Addr;

typedef struct requestinfo{
    char method  [10];
    char * fname ;
    int contentlen ;
}RequestInfo;



int Listenfd(const char * port);
int Serve_file(const char * filename , int socketfd);

RequestInfo ParseRequest(int socketfd);
void NotImplemented(int socketfd);

void Response(CompressedMatrix * result , int socketfd);


void NormalEmptyResponse(int fd);
void OperationErrorResponse(int fd);


#endif
