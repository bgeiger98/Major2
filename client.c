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
void clientHandler(int argc, char *argv[]);
void serverHandler(int argc, char *argv[]);

int main(int argc, char *argv[])
{
	pthread_t serverThread;
	pthread_t clientThread;
	
	if(pthread_create(&serverThread, NULL, serverHandler, (void*) 1) < 0 )
	{
		perror("thread");
		exit(1);
	}
	pthread_join(serverThread, NULL); //Waits for server connection to be finished before moving on 
	if(pthread_create(&clientThread, NULL, clientHandler, (void *) 2) < 0)
	{
		perror("thread");
		exit(1);
	}
	pthread_join(clientThread, NULL);
}

void clientHandler(int argc, char *argv[])
{
	int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    
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
    
    while(1) {
            if ((n = write(sockfd,buffer,strlen(buffer))) < 0)
            {
                error("ERROR writing to socket");
            }
    
            if ((n = read(sockfd,buffer,255)) < 0)
            {
                error("ERROR reading from socket");
            }
    }
	close(sockfd);
}


void serverHandler(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    
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
    
    while(1) {
            if ((n = write(sockfd,buffer,strlen(buffer))) < 0)
            {
                error("ERROR writing to socket");
            }
    
            if ((n = read(sockfd,buffer,255)) < 0)
            {
                error("ERROR reading from socket");
            }
    }

    close(sockfd); 
}
