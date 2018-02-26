/*
 * Echo klientas
 * 
 * Author: K�stutis Mizara
 * Description: I�siun�ia serveriui prane�im� ir j� gauna
 */

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFLEN 1024

int main(int argc, char *argv[]){
    unsigned int port;
    int s_socket;
    struct sockaddr_in servaddr; // Serverio adreso struktura
    fd_set read_set;

    char recvbuffer[BUFFLEN];
    char sendbuffer[BUFFLEN];

    int i;

    if (argc != 3){
        fprintf(stderr,"USAGE: %s <ip> <port>\n",argv[0]);
        exit(1);
    }

    port = atoi(argv[2]);

    if ((port < 1) || (port > 65535)){
        printf("ERROR #1: invalid port specified.\n");
        exit(1);
    }

    /*
     * Sukuriamas socket'as
     */
    if ((s_socket = socket(AF_INET, SOCK_STREAM,0))< 0){
        fprintf(stderr,"ERROR #2: cannot create socket.\n");
        exit(1);
    }
                                
   /*
    * Isvaloma ir uzpildoma serverio struktura
    */
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET; // nurodomas protokolas (IP)
    servaddr.sin_port = htons(port); // nurodomas portas
    
    /*
     * Isverciamas simboliu eilutuje uzrasytas ip i skaitine forma ir
     * nustatomas serverio adreso strukt�roje.
     */
    if ( inet_aton(argv[1], &servaddr.sin_addr) <= 0 ) {
        fprintf(stderr,"ERROR #3: Invalid remote IP address.\n");
        exit(1);
    }

    /* 
     * Prisijungiama prie serverio
     */
    if (connect(s_socket,(struct sockaddr*)&servaddr,sizeof(servaddr))<0){
        fprintf(stderr,"ERROR #4: error in connect().\n");
        exit(1);
    }
    printf("Successfully connected!\n"); 

    memset(&sendbuffer,0,BUFFLEN);

    fcntl(0,F_SETFL,fcntl(0,F_GETFL,0)|O_NONBLOCK); //No clue what this does
    while (1){
        FD_ZERO(&read_set);
        FD_SET(s_socket,&read_set);
        FD_SET(0,&read_set);

        select(s_socket+1,&read_set,NULL,NULL,NULL);

        //TODO check if server died

        if (FD_ISSET(s_socket, &read_set)){ //if got a signal from server
            memset(&recvbuffer,0,BUFFLEN);
            i = read(s_socket, &recvbuffer, BUFFLEN);
            printf("Response from server: %s\n",recvbuffer);
        }
        else if (FD_ISSET(0,&read_set)) {   //else std_in ops
            i = read(0,&sendbuffer, BUFFLEN);
            write(s_socket, sendbuffer,i);
        }
    }

//    printf("Enter the message: ");
//    fgets(buffer, BUFFLEN, stdin);
    /*
     * Issiunciamas pranesimas serveriui
     */
//    send(s_socket,buffer,strlen(buffer),0);

//    memset(&buffer,0,BUFFLEN);
    /*
     * Pranesimas gaunamas is serverio
     */
//    recv(s_socket,&buffer,BUFFLEN,0);
//    printf("Server sent: %s\n", buffer);

    /*
     * Socket'as u�daromas
     */
    close(s_socket);
    return 0;
}
