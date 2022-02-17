#include "netheader.h"
#include "header.h"

#include <errno.h>
#include <string.h>

int main(int argc , char ** argv ){
    if(argc != 2){
        fprintf(stderr , "usage : server port\n");
        exit(0);
    }

    int listenfd = Listenfd(argv[1]);

    if(listenfd < 0){
        fprintf(stderr, "listen error\n");
        exit(0);
    }

    int connfd ;
                                                
    const int BUFsize = 1000;

    char clientname [BUFsize];
    char clientport [BUFsize];

    CompressedMatrix * m1 = NULL ;
    CompressedMatrix * m2 = NULL ;


    while(1){

        char path [BS] = "../WebPage";

        CompressedMatrix * res = NULL ;

        struct sockaddr clientaddr ;
        socklen_t clientlen = sizeof(struct sockaddr);

        connfd = accept(listenfd, &clientaddr, &clientlen);

        fprintf(stderr , "start getnameinfo !!! \n");
        int errornumber = getnameinfo(&clientaddr, clientlen, clientname, BUFsize, clientport, BUFsize, 0);
        if (errornumber != 0){
            const char * message = gai_strerror(errornumber);
            fprintf(stderr , "Error in getnameinfo : %s\n" , message);
            exit(0);
        }
        fprintf(stderr , "\nerrornumber : %d\n" , errornumber );
        printf("Accept connection from [%s : %s]\n", clientname , clientport);


        RequestInfo info = ParseRequest(connfd);

        // check if it is a GET request
        if (strcmp("GET", info.method) != 0){
            //If not a GET, check if it is a POST request
            if(strcmp("POST", info.method) == 0){
                //Handle POST
                int datalen = info.contentlen ;
                printf("POST payload length : %d\n" , datalen);

                if (datalen > 0){

                    int DoNotDestroy = 0 ;

                    char * data = (char*)malloc(sizeof(char) * ( datalen) + 1);
                    int readret = read(connfd, data, datalen);
                    *(data + datalen) = '\0';
                    write(1, data, readret);

                    char op [3] ;
                    sscanf(data, "%3s" , op);


                    if (strcmp("crt", op) == 0){

                        DoNotDestroy = 1;
                        int num = 0 ;
                        sscanf(data + 3 , "%d" , &num);
                        if (num == 1){
                            m1 = (CompressedMatrix*)malloc(sizeof(CompressedMatrix));
                            Input( m1  , data + 5);
                            Sort(m1);
                            Response(m1 , connfd);
                        }
                        else if (num == 2){
                            m2 = (CompressedMatrix*)malloc(sizeof(CompressedMatrix));
                            Input( m2 , data + 5);
                            Sort(m2);
                            Response(m2 , connfd);
                        }
                    }
                    else if (strcmp("add", op) == 0){
                        res= Addsub(m1, m2, ADD);
                        if (res == NULL ){ OperationErrorResponse(connfd); }
                    }
                    else if (strcmp("sub" , op) == 0){
                        res= Addsub(m1, m2, SUB);
                        if (res == NULL ){ OperationErrorResponse(connfd); }
                    }
                    else if (strcmp("mul", op) == 0){
                        res = Multiply(m1, m2);
                        if (res == NULL ){ OperationErrorResponse(connfd); }
                    }
                    else if (strcmp("tas", op) == 0){
                        int num = 0 ;
                        sscanf(data + 3 , "%d" , &num);
                        if (num == 1){
                            res = Transpose(m1);
                        }
                        else if (num == 2){
                            res = Transpose(m2);
                        }
                    }
                    else if (strcmp("clr", op) == 0){
                        DoNotDestroy = 1;
                        if (m1) { Destroy(m1); m1 = NULL ; }
                        if (m2) { Destroy(m2); m2 = NULL ;}
                        NormalEmptyResponse(connfd);
                    }

                    else {
                        NotImplemented(connfd);
                    }

                    if (!DoNotDestroy){ if (res){ Response(res , connfd); Destroy(res); res = NULL ;} }
                    free(data);
                }
            }
            else{
                //Neither a GET nor a POST
                NotImplemented(connfd);
            }
        }
        else{
            //Handle GET
            if (strcmp("/", info.fname) == 0){
                strcat(path, "/index.html");
            }
            else{
                strcat(path, info.fname);
            }

            Serve_file(path, connfd);
        }

        free(info.fname);
        close(connfd);
    }
}
