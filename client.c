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
#include <stdbool.h>
#include <sys/poll.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

void startSubClient(void*);
void startSubServer(void*);

    pthread_t serverThread;
    pthread_t clientThread;
int main(int argc, char *argv[])
{
    int sockfd, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buf[1024];                /* Buffer for messages to others. */
    char msg[1024];
    int nread;                      /* # chars on read()*/
    int nready;                     /* # descriptors ready. */
    fd_set fds;                     /* Set of file descriptors to poll*/
   // struct pollfd fds[2];
    int i;
    int j;
    bool svrT = 0, cliT = 0;    
    if (argc < 4)
    {
        fprintf(stderr,"usage %s hostname port ipAddress port\n", argv[0]);
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
	int flag3 = 0, flag4 = 0;    
	for(j = 0; j < 2; j++) {
	    int flag = 1;
	    svrT = 0;
	    cliT = 0;
	    while(flag == 1) {
		printf("Begin while\n");
		printf("Flag3 = %i\n Flag4 = %i\n", flag3, flag4);
		fflush(stdin);
		fflush(stdout);
		FD_ZERO(&fds);
		FD_SET(sockfd,&fds);
		FD_SET(STDIN_FILENO,&fds);
		nready = select(sockfd+1, &fds, (fd_set *) 0, (fd_set *) 0, (struct timeval *) 0);
		if (nready == -1) {
		    perror("select");
		    exit(1);
		}
		flag3 = 0;
		if (FD_ISSET(STDIN_FILENO, &fds) && flag3 == 0) { // client 1
		    flag=0;
		    flag3 = 1;
		    printf("In client 1 before gets\n");
		    fgets(msg, sizeof(msg), stdin);
		    send(sockfd, msg, sizeof(msg), 0); //send pq
		    svrT = 1;
		    nread = recv(sockfd, buf, sizeof(buf), 0); // receive ne
	//printf("nread = %i\n", nread);
		    pthread_create(&serverThread, NULL, (void *) &startSubServer, (void *) argv);
		}
		if (FD_ISSET(sockfd, &fds) && flag4 == 0) { // client 2
		    flag=0;
		    flag4 = 1;
		    flag3 = 0;
		    printf("In client 2 before receive\n");
	//printf("nread = %i\n", nread);
		    cliT = 1;
		    if (flag3 = 0)
		    	nread = recv(sockfd, buf, sizeof(buf), 0);
		    sleep(1);
		    if (flag3 = 1) {
			while((nread = recv(sockfd, buf, sizeof(buf), 0)) < 1) {};
		    }

		    pthread_create(&clientThread, NULL, (void *) &startSubClient, (void *) argv);
		}
		for (i = 0; i < 1024; i++) {
		    buf[i] = '\0';
		}
	    }
	    printf("Out of while\n");
	    if (svrT == 1)
		pthread_join(serverThread, NULL);
	    if (cliT == 1)
		pthread_join(clientThread, NULL); 
	}
   
    close(sockfd);
    return 0;
    
}

void startSubClient(void *argv) {
    int sockfd2, portno2, n2;
    struct sockaddr_in serv_addr2;
    struct hostent *server2;
    char buffer2[1024];
    
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
    for (i = 0; i < 1024; i++){
        buffer2[i] = '\0';
    }
    if ((n2 = recv(sockfd2,buffer2,sizeof(buffer2), 0)) < 1)
    {
        error("ERROR buffer 2 writing to socket");
    }
    printf("buffer2 = %s\n", buffer2);
    close(sockfd2);
    pthread_exit(NULL);
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
    for (i = 0; i < 1024; i++){
        buf[i] = '\0';
    }
    wait(1);
    gets(buf);
    if((nread = write(ns, buf, sizeof(buf))) < 0)
    {
        /* If error or eof, terminate. */
        close(ns);
    }
    close(ns);
    close(s);
    pthread_exit(NULL);
}

