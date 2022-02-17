#include "netheader.h"

int Listenfd(const char * port){
    Addr hints, *listp , *p;

    int  optval = 1;

    //init hints
    memset(&hints, 0, sizeof(Addr));
    hints.ai_socktype = SOCK_STREAM ;
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG | AI_NUMERICSERV ;

    //Servers can have multiple IP addresses
    //Since we don't know the IP of my localhost, we must query the DNS server
    //get a list of [serverIP : serverPort]

    getaddrinfo(NULL, port, &hints ,&listp);

    int socketfd;
    for (p = listp ; p ; p = p->ai_next){
        //create a socket with queried data
        if ( ( socketfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol) ) < 0){
            //if failed, find next
            continue;
        }

        if ( bind(socketfd, p->ai_addr , p->ai_addrlen) == 0 ){
            break;
        }
        //bind failed, close socketfd and try next
        close(socketfd);
    }

    //all trial failed
    if(!p){
        return -1;
    }

    if (listen(socketfd, 256) < 0){
        //listen failed
        close(socketfd);
        return -1;
    }

    freeaddrinfo(listp);
    return socketfd;
}


/*----------------------------file serving -------------------------*/
static void send_ok_header(int socketfd){
   char header [] = "HTTP/1.1 200 OK\r\nConnection: close\r\n\r\n";
   int headerlen = strlen(header);
   for (int sent = 0 ; sent < headerlen ; ){
       sent += send(socketfd,header+sent,headerlen-sent , 0);
   }
}
static void send_error(int socketfd){
   char content [] = "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n<html><body><h1>File Not Found</h1></body></html>";
   int headerlen = strlen(content);
   for (int sent = 0 ; sent < headerlen ; ){
       sent += send(socketfd,content+sent,headerlen-sent , 0);
   }
}


void NotImplemented(int socketfd){
   char content [] = "HTTP/1.1 501 Not Implemented\r\nConnection: close\r\n\r\n<html><body><h1>Method not implemented yet!</h1><h2>We only accept GET and POST.</h2></body></html>";
   int headerlen = strlen(content);
   for (int sent = 0 ; sent < headerlen ; ){
       sent += send(socketfd,content+sent,headerlen-sent , 0);
   }
}

void NormalEmptyResponse(int fd){
    send_ok_header(fd);
}

void OperationErrorResponse(int fd){
   char con [] = "HTTP/1.1 500 Internal Server Error\r\nConnection: close\r\n\r\nDimension error";
   int headerlen = strlen(con);
   for (int sent = 0 ; sent < headerlen ; ){
       sent += send(fd,con+sent,headerlen-sent , 0);
   }

}


int Serve_file(const char * filename , int socketfd){

    FILE * file = fopen(filename, "r");
    if(file == NULL ){
        fprintf(stderr, "file not found\n");
        send_error(socketfd);
        return -1;
    }
    send_ok_header(socketfd);

    char buf [BS];

    while(fgets(buf, BS, file)){
        printf("%s" , buf);
        send(socketfd,buf,strlen(buf), 0);
    }
    fclose(file);
    printf("<--------------- Next --------------->\n");
    return 0;
}

static char * getheaderline(int socketfd){
    char* res = (char*)malloc(sizeof(char) * BS);
    char* walk = res;
    char c ;
    int cot = 0 ;
    while(recv(socketfd, &c, sizeof(char), 0) > 0){
        if( c == '\r' ){
            continue;
        }
        if (c == '\n' || cot >= BS - 1){
            break;
        }
        *walk++ = c ;
        cot++; 
    }
    *walk = '\0';
    if (walk == res){
        free(res);
        res = NULL ;
    }
    return  res ;
}

RequestInfo ParseRequest(int socketfd){
    char * line ;
    char * res = (char*)malloc(sizeof(char)*BS);
    RequestInfo ret ;
    ret.contentlen = -1;
    ret.fname = res;
    int firstlineget = 0;
    char key [BS];
    char value [BS];

    while( (line = getheaderline(socketfd)) != NULL ){
        if (firstlineget == 0){
            firstlineget = 1 ;
            sscanf(line, "%s %s" , ret.method , res);
        }
        else{
            sscanf(line , "%1024s %1024s" , key , value);
            if ( strcmp(key, "Content-Length:")  == 0){
                ret.contentlen = atoi(value);
            }
        }

        printf("%s\n" , line);
        free(line);
    }

    return ret;
}

void Response(CompressedMatrix * result , int socketfd){
    int end = result->NonZeroNum ;
    Item* start = result->location;
    char * buffer = (char*)calloc(sizeof(Item) , end * 10);
    char * buf = buffer;
    char temp [BS];

    sprintf(temp , "%d %d %d\n" , result->row , result->column , end );
    int templen = strlen(temp);
    memcpy(buf, temp, templen);
    for (int i = 0 ; i < end ; i++){
        buf += templen;
        sprintf(temp , "%d %d %d\n" ,start[i].row , start[i].col , start[i].element );
        templen = strlen(temp);
        memcpy(buf, temp , templen);
    }

    buf+=templen;

    fprintf(stderr , "\ndistance : %ld , real size : %ld\n" , buf - buffer , end * sizeof(Item) * 10);

    *buf = '\0';

    send_ok_header(socketfd);

    int len = strlen(buffer);
    write(1,buffer,len);
    len -= send(socketfd, buffer,len, 0);
    while(len){
        len -= send(socketfd, buffer,len, 0);
    }
    free(buffer);
}

