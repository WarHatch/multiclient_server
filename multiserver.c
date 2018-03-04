#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <string.h>
#include <unistd.h>

#include "input_ops.c"
#include "reminder_list.c"

#define BUFFLEN 1024
#define MAXCLIENTS 5
#define DATEFORMAT "%y/%m/%d %H/%M/%S"

int findemptyuser(int c_sockets[]){
    int i;
    for (i = 0; i <  MAXCLIENTS; i++){
        if (c_sockets[i] == -1){
            return i;
        }
    }
    return -1;
}

int main(int argc, char *argv[]){
    unsigned int port;
    unsigned int clientaddrlen;
    int master_socket; //or listening_socket
    int c_sockets[MAXCLIENTS];
    fd_set read_set;

    struct sockaddr_in servaddr;
    struct sockaddr_in clientaddr;

    int maxfd = 0;
    int i;

    char buffer[BUFFLEN];
    char* response;

    struct node* rem_list = NULL;

    if (argc != 2){
        fprintf(stderr, "USAGE: %s <port>\n", argv[0]);
        return -1;
    }

    port = atoi(argv[1]);
    if ((port < 1) || (port > 65535)){
        fprintf(stderr, "ERROR #1: invalid port specified.\n");
        return -1;
    }

    if ((master_socket = socket(AF_INET, SOCK_STREAM,0)) < 0){
        fprintf(stderr, "ERROR #2: cannot create listening socket.\n");
        return -1;
    }

    /*
     * Isvaloma ir uzpildoma serverio struktura
     */
    memset(&servaddr,0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(port);

    if (bind (master_socket, (struct sockaddr *)&servaddr,sizeof(servaddr))<0){
        fprintf(stderr,"ERROR #3: bind listening socket.\n");
        return -1;
    }

    /*
     * master_socket klauso ir turi reaguoti i naujus prisijungimus
     */
    if (listen(master_socket, 5) <0){
        fprintf(stderr,"ERROR #4: error in listen().\n");
        return -1;
    }
    int opt = 1; //TRUE
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )  
    {  
        fprintf(stderr,"ERROR #5: error setting socket options.\n");
        return -1;  
    }  

    printf("The server is listening...\n");                         

    for (i = 0; i < MAXCLIENTS; i++){
        c_sockets[i] = -1;
    }


    for (;;){
        FD_ZERO(&read_set);
        for (i = 0; i < MAXCLIENTS; i++){
            if (c_sockets[i] != -1){
                FD_SET(c_sockets[i], &read_set);
                if (c_sockets[i] > maxfd){
                    maxfd = c_sockets[i];
                }
            }
        }

        FD_SET(master_socket, &read_set);
        if (master_socket > maxfd){
            maxfd = master_socket;
        }
        
        select(maxfd+1, &read_set, NULL , NULL, NULL);

        if (FD_ISSET(master_socket, &read_set)){ //a new connection is incoming
            int client_id = findemptyuser(c_sockets);
            if (client_id != -1){
                clientaddrlen = sizeof(clientaddr);
                memset(&clientaddr, 0, clientaddrlen);
                c_sockets[client_id] = accept(master_socket, 
                    (struct sockaddr*)&clientaddr, &clientaddrlen);
                printf("Connected:  %s\n",inet_ntoa(clientaddr.sin_addr));
            }
        }
        for (i = 0; i < MAXCLIENTS; i++){
            if (c_sockets[i] != -1){
                if (FD_ISSET(c_sockets[i], &read_set)){ //===== If got a message from socket
                    memset(&buffer,0,BUFFLEN);
                    int r_len = recv(c_sockets[i],&buffer,BUFFLEN,0);

                    memset(&clientaddr, 0, clientaddrlen);
                    getpeername(c_sockets[i], (struct sockaddr*)&clientaddr, &clientaddrlen);
                    printf("\nReceived. %s : '%s'", inet_ntoa(clientaddr.sin_addr), buffer);

                    if (r_len <= 0){ //Sender disconnected
                        printf("---> Assuming the client has disconnected.\n");
                        close(c_sockets[i]);
                        c_sockets[i] = -1;
                    }

                    //=== Looking for commands (ADD or REM or SHOW or EXIT) ===
                    else if (r_len >= 4) {
                        //bzero(response, BUFFLEN);
                        response = calloc(BUFFLEN, sizeof(char));

                        char* command;
                        command = substring(buffer, 1, 4);

                        if (strcmp(command,"ADD ") == 0){
                            printf("ADD command gotten\n");
                            char* remainingArgs = buffer +4;
                            
                            char name[50];
                            remainingArgs = getParameter(remainingArgs, name);
                            printf("Name arg: '%s'\n", name);

                            //printf("leftover args line: %sEND\n", remainingArgs);
                            //printf("stopped on symbol: %c\n", remainingArgs[0]);
                            if (*remainingArgs == '+'){
                                char time_s[20];
                                struct tm para_tm;
                                bzero(&para_tm, sizeof(struct tm)); //IMPORTANT this prevents incorrect hour value

                                remainingArgs = getParameter(remainingArgs+1, time_s);
                                printf("Time arg: '%s'\n", time_s);

                                if (strptime(time_s, DATEFORMAT, &para_tm) == NULL){
                                    fprintf(stderr, "NOTICE: Invalid parameter format\n");
                                    response = "NOTICE: Invalid parameter format\n";
                                }
                                else{
                                    time_t a_time = mktime(&para_tm);
                                    time_t now = time(0);
                                    struct tm * timeinfo;
                                    timeinfo = localtime ( &now ); 
                                    printf ( "Current local time and date: %s", asctime (timeinfo) );
                                    timeinfo = localtime ( &a_time );
                                    printf ( "Submited time and date: %s", asctime (timeinfo) );

                                    double diffSecs = difftime(a_time, now); //Checks if reminder is late or not 
                                    printf ( "Timediff: %f\n", diffSecs );

                                    if (rem_list == NULL)
                                        rem_list = create(name, a_time, "");
                                    else
                                        addReminder(rem_list, name, a_time, "");

                                    lastElementIndex(rem_list);
                                    response = "You have successfully added a reminder\n";
                                }
                            }
                        }
                        else if (strcmp(command,"REM ") == 0){
                            printf("REM command gotten\n");
                            response = "NOTICE: Removing is not yet implemented.\n";
                        }
                        else if (strcmp(command,"SHOW") == 0){
                            printf("SHOW command gotten\n");
                            listToString(rem_list, response, BUFFLEN); //Unhandled error
                        }
                        else{
                            printf("NOTICE: Invalid command.\n");
                            response = "NOTICE: Invalid command.\n";
                        }
                    }

                    //Sends a response to client
                    int w_len = send(c_sockets[i], response, BUFFLEN,0); // <---- sending

                    
                    /*
                    else {
                        int j;
                        for (j = 0; j < MAXCLIENTS; j++){
                            if (c_sockets[j] != -1){
                                int w_len = send(c_sockets[j], buffer, r_len,0); // <---- sending
                                if (w_len <= 0){ //Receiver is disconnected
                                    printf("---> Assuming the client has disconnected.\n");
                                    close(c_sockets[j]);
                                    c_sockets[j] = -1;
                                }
                            }
                        }
                    }
                    */
                }
            }
        }
    } //end of 'for(;;)' loop

    return 0;
}