// ****************
// * Major 2 cli. *
// * Group 19.    *     
// ****************

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n = 123;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int isClient = 0, isServer = 0;
    
    char buffer[256];
    
    if (argc < 3)
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
        error("ERROR connecting");
    }
    
    if ((n = read(sockfd,buffer,255) < 0)
    {
        error("ERROR reading from socket");
    }
        
    if (buffer == "client") {
        
    }
    while(1) {
        if ((n = read(sockfd,buffer,255)) < 0)
        {
            error("ERROR reading from socket");
        }
        if (isClient == 1) {
            int sockfd2, portno2, n2;
            struct sockaddr_in serv_addr2;
            struct hostent *server2;
            
            char buffer2[256];
            
            if (argc < 5)
            {
                fprintf(stderr,"usage %s hostname port clientHostname port\n", argv[0]);
                exit(0);
            }
            
            portno2 = atoi(argv[4]);
            sockfd2 = socket(AF_INET, SOCK_STREAM, 0);
            
            if (sockfd2 < 0)
            {
                error("ERROR opening socket");
            }
            
            server2 = gethostbyname(argv[3]);
            
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
                error("ERROR connecting");
            }
            
            while(1) {
                if ((n2 = write(sockfd2,buffer2,strlen(buffer2))) < 0)
                {
                    error("ERROR writing to socket");
                }
            }
            close(sockfd2);
        }
        if (isServer == 1) {
            char buf[1024];                /* Buffer for messages to others. */
            char msg[1024];
            int s;                          /* Listen socket */
            int ns;                         /* Socket for first connection. */
            int len;                        /* len of sockaddr */
            int maxfd;                      /* descriptors up to maxfd-1 polled*/
            int nread;                      /* # chars on read()*/
            int nready;                     /* # descriptors ready. */
            int portnoS;
            struct sockaddr_in serv_addrS, cli_addr1;
            socklen_t clilen1;
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
            bzero((char *) &serv_addrS, sizeof(serv_addrS));
            portnoS = atoi(argv[4]);
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
            maxfd = (ns > ns2 ? ns : ns2) + 1;    //???????????
            
            while (1)
            {
                /* Wait for some input. */
                nready = select(maxfd, &fds, (fd_set *) 0, (fd_set *) 0, (struct timeval *) 0);
               
                /* If either descriptor has some input,
                 read it and copy it to the other. */
                for (i = 0; i < 1023; i++)
                    buf[i] = '\0';
                
                if( FD_ISSET(ns, &fds))
                {
                    nread = recv(ns, buf, sizeof(buf), 0);
                    if(nread < 1)
                    {
                        /* If error or eof, terminate. */
                        close(ns);
                    }
                }
                
            }
        } 
    }
    
    close(sockfd);
    return 0;
    }
        
