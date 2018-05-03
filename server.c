#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <netinet/in.h>
#include "krypt.h"

int main(int argc, char *argv[])
{
    char buf[1024];                /* Buffer for messages to others. */
    char msg[1024];
    int s;                          /* Listen socket */
    int ns;                         /* Socket for first connection. */
    int ns2;                        /* Socket for second connection. */
    int len;                        /* len of sockaddr */
    int maxfd;                      /* descriptors up to maxfd-1 polled*/
    int nread;                      /* # chars on read()*/
    int nready;                     /* # descriptors ready. */
    int portno;
    struct sockaddr_in serv_addr, cli_addr1, cli_addr2;
    socklen_t clilen1, clilen2;
    fd_set fds;                     /* Set of file descriptors to poll*/
    time_t t;
    srand((unsigned) time(&t));
    int i;
    
    /* Create the socket. */
    
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        exit(1);
    }
    
    /* Create the address of the server.*/
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    
    
    /*Bind the socket to the address.*/
    int on = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (bind(s, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
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
    else {
	printf("Connection 1 accepted.\n");
    }
    clilen2 = sizeof(cli_addr2);
    /* Accept another connection. */
    if ((ns2 = accept(s, (struct sockaddr *) &cli_addr2, &clilen2)) < 0)
    {
        perror("accept");
        exit(1);
    }
    else {
	printf("Connection 2 accepted.\n");
    }
    
    maxfd = (ns > ns2 ? ns : ns2) + 1;
    int flag = 1;
    while(flag == 1)
    {
        /* Set up polling using select. */
        FD_ZERO(&fds);
        FD_SET(ns,&fds);
        FD_SET(ns2,&fds);
	
	printf("above select\n");        
        /* Wait for some input. */
        nready = select(maxfd, &fds, (fd_set *) 0, (fd_set *) 0, (struct timeval *) 0);
	printf("below connect\n");
        /* If either descriptor has some input,
         read it and copy it to the other. */
        for (i = 0; i < 1023; i++){
            buf[i] = '\0';
        }
        if( FD_ISSET(ns, &fds))
        {
	    printf("ns1 flag\n");
	    flag = 0;
            nread = recv(ns, buf, sizeof(buf), 0);
            /* If error or eof, terminate. */
            if(nread < 1)
            {
                close(ns);
                close(ns2);
            }
            send(ns, buf, sizeof(buf), 0);
            
            send(ns2, buf, sizeof(buf), 0);
        }
        
        for (i = 0; i < 1023; i++) {
            buf[i] = '\0';
	}
        
        
        if( FD_ISSET(ns2, &fds))
        {
	    printf("ns2 flag\n");
	    flag = 0;
            nread = recv(ns2, buf, sizeof(buf), 0);
            /* If error or eof, terminate. */
            if(nread < 1)
            {
                close(ns);
                close(ns2);
            }
            send(ns2, buf, sizeof(buf), 0);
            
            send(ns, buf, sizeof(buf), 0);
        }
	for (i = 0; i < 1023; i++){
            buf[i] = '\0';
	}

    }
    close(ns);
    close(ns2);
}


