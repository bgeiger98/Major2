#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <semaphore.h>
#include "krypt.h"

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void startSubClient(void*);
void startSubServer(void*);

int main(int argc, char *argv[])
{
    int sockfd, portno, n = 123;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int isClient = 0;
    char buf[1024];                /* Buffer for messages to others. */
    char msg[1024];
    int len;                        /* len of sockaddr */
    int maxfd;                      /* descriptors up to maxfd-1 polled*/
    int nread;                      /* # chars on read()*/
    int nready;                     /* # descriptors ready. */
    fd_set fds;                     /* Set of file descriptors to poll*/
    time_t t;
    int i;
    char buffer[256];
    pthread_t serverThread;
    pthread_t clientThread;
    
    if (argc < 4)
    {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        exit(0);
    }
    
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sockfd < 0)
    {
        error("ERROR opening socket");
    }
    
    server = gethostbyname(argv[1]);
    
    if (server == NULL)
    {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
        error("1ERROR connecting");
    }
    
    /*if ((n = read(sockfd,buffer,255) < 0))
    {
        error("ERROR reading from socket");
        exit(0);
    }*/
    int flag = 1;
    while(flag == 1) {
        FD_ZERO(&fds);
        FD_SET(sockfd,&fds);
        FD_SET(0,&fds);
        nready = select(sockfd+1, &fds, (fd_set *) 0, (fd_set *) 0, (struct timeval *) 0);
	if (nready == -1) {
		perror("select");
		exit(1);
	}
        if (FD_ISSET(0, &fds)) { // client 1
	    flag=0;
	    printf("In client 1 before gets\n");
            fgets(msg, strlen(msg), stdin);
            send(sockfd, msg, sizeof(msg), 0); //send pq
            nread = recv(sockfd, buf, sizeof(buf), 0); // receive ne
            pthread_create(&serverThread, NULL, (void *) &startSubServer, (void *) argv);
        }
        else if (FD_ISSET(sockfd, &fds)) { // client 2
	    flag=0;
	    printf("In client 2 before receive\n");
            nread = recv(sockfd, buf, sizeof(buf), 0);
            sleep(1);
            pthread_create(&clientThread, NULL, (void *) &startSubClient, (void *) argv);
        }
	for (i = 0; i < 1024; i++) {
		buf[i] = '\0';
	}
    }
    pthread_join(serverThread, NULL);
    pthread_join(clientThread, NULL);
    flag = 1;
     fflush(stdin);
     while(flag == 1) {
        FD_ZERO(&fds);
        FD_SET(sockfd,&fds);
        FD_SET(0,&fds);
        nready = select(sockfd+1, &fds, (fd_set *) 0, (fd_set *) 0, (struct timeval *) 0);
	if (nready == -1) {
		perror("select");
		exit(1);
	}
        if (FD_ISSET(0, &fds)) { // client 1
	    flag=0;
	    printf("In client 1 before gets\n");
            fgets(msg, strlen(msg), stdin);
            send(sockfd, msg, sizeof(msg), 0); //send pq
            nread = recv(sockfd, buf, sizeof(buf), 0); // receive ne
            pthread_create(&serverThread, NULL, (void *) &startSubServer, (void *) argv);
        }
        else if (FD_ISSET(sockfd, &fds)) { // client 2
	    flag=0;
	    printf("In client 2 before receive\n");
            nread = recv(sockfd, buf, sizeof(buf), 0);
            sleep(1);
            pthread_create(&clientThread, NULL, (void *) &startSubClient, (void *) argv);
        }
    }
    pthread_join(serverThread, NULL);
    pthread_join(clientThread, NULL);

    close(sockfd);
    return 0;
        
}

void startSubClient(void *argv) {
    int sockfd2, portno2, n2;
    struct sockaddr_in serv_addr2;
    struct hostent *server2;
    char buffer2[256] = "HEllo\n";
    
    int value = ((int*) argv)[4];
    portno2 = atoi(value);
    sockfd2 = socket(AF_INET, SOCK_STREAM, 0);
    
    if (sockfd2 < 0)
    {
        error("ERROR opening socket");
    }
    value = ((int*) argv)[3];
    server2 = gethostbyname(value);
    
    if (server2 == NULL)
    {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    
    bzero((char *) &serv_addr2, sizeof(serv_addr2));
    serv_addr2.sin_family = AF_INET;
    bcopy((char *)server2->h_addr, (char *)&serv_addr2.sin_addr.s_addr, server2->h_length);
    serv_addr2.sin_port = htons(portno2);
    
    if (connect(sockfd2,(struct sockaddr *) &serv_addr2,sizeof(serv_addr2)) < 0)
    {
        error("2ERROR connecting");
    }
    else 
	printf("Sub Client connected\n");
    
    int i;
    for (i = 0; i < 256; i++){
	buffer2[i] = '\0'; 
    }
    if ((n2 = recv(sockfd2,buffer2,sizeof(buffer2), 0)) < 1)
    {
        error("ERROR buffer 2 writing to socket");
    }
    printf("buffer2 = %s\n", buffer2);
    close(sockfd2);
}
void startSubServer(void *argv) {
    int portnoS;
    struct sockaddr_in serv_addrS, cli_addr1;
    int s;                          /* Listen socket */
    int ns;                         /* Socket for first connection. */
    socklen_t clilen1;
    int i;
    char buf[1024];                /* Buffer for messages to others. */
    int nread;
    /* Create the socket. */
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        exit(1);
    }
    
    int value = ((int*) argv)[4];
    /* Create the address of the server.*/
    bzero((char *) &serv_addrS, sizeof(serv_addrS));
    portnoS = atoi(value);
    serv_addrS.sin_family = AF_INET;
    serv_addrS.sin_addr.s_addr = INADDR_ANY;
    serv_addrS.sin_port = htons(portnoS);
    
    
    /*Bind the socket to the address.*/
    int on = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (bind(s, (struct sockaddr *) &serv_addrS, sizeof(serv_addrS)) < 0)
    {
        error("ERROR on binding");
        exit(1);
    }
    
    /* Listen for connections. */
    if (listen(s, 5) < 0)
    {
        perror( "listen");
        exit(1);
    }
    clilen1 = sizeof(cli_addr1);
    
    /*Accept a connection.*/
    if ((ns = accept(s, (struct sockaddr *) &cli_addr1, &clilen1)) < 0)
    {
        perror("accept");
        exit(1);
    }
    else 
	printf("Accepted connection\n");
 //   maxfd = (ns > ns2 ? ns : ns2) + 1;    //???????????
    
        
    /* If either descriptor has some input,
        read it and copy it to the other. */
    for (i = 0; i < 1023; i++){
        buf[i] = '\0';
    }
    wait(1);
    gets(buf);
    if((nread = write(ns, buf, strlen(buf))) < 0)
    {
    /* If error or eof, terminate. */
 	close(ns);
    }
    close(ns);
    close(s);
}
