// ****************
// * Major 2      *
// * Group 19     *
// ****************

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>

    int s;                          /* Listen socket */
	int ns = 0;
	int ns2 = 0;
	int numConnect;
void connectHandler();
void listenInf();


int main(int argc, char *argv[])
{
    char buf[1024];                /* Buffer for messages to others. */
    char msg[1024];

//    int ns;                         /* Socket for first connection. */
//    int ns2;                        /* Socket for second connection. */
    int len;                        /* len of sockaddr */
                    /* # descriptors ready. */
    int portno;
    struct sockaddr_in serv_addr, cli_addr1, cli_addr2;
    socklen_t clilen1, clilen2;

    int i;
	pthread_t lid;
	pthread_t cid;
    
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
    	printf("1)%d\n", numConnect);
		
	
	if(pthread_create(&lid, NULL, (void*) &listenInf, (void*) 0) < 0)
	{
		perror("infinite connections");
		exit(1);
	}
	printf("2)%d\n", numConnect);
	while(numConnect < 2)
	{
		//Wait for 2 connections
//			printf("%d\n", numConnect);
	}
		printf("3)%d\n", numConnect);
		
	if(pthread_create(&cid, NULL, (void*) &connectHandler, (void*) 0) < 0)
	{
		perror("thread");
		exit(1);
	}
	printf("Time to Connect Handler\n");

	printf("skipped connectHandler\n");
	
	pthread_join(lid, NULL);
	pthread_join(cid, NULL);
	close(s);
}

void listenInf()
{
	int temp;
	struct sockaddr_in cli_addr;
	int clilen = sizeof(cli_addr);
	char *message;
//	int numConnect = 0;
	
	while(1)
	{
		if((temp = accept(s, (struct sockaddr*) &cli_addr, &clilen)) < 0)
		{
			perror("accept");
			exit(1);
		}
		if(ns == 0)
		{
			printf("HELLO1\n");
			ns = temp;
			message = "First Client is connected\n";
			write(ns, message, strlen(message));
			numConnect ++;
		}
		else if(ns2 == 0)
		{
			printf("HELLO2\n");
			ns2 = temp;
			message = "Second Client is connected\n";
			write(ns2, message, strlen(message));
			numConnect ++;
		}
		else
		{
			message = "There are no connections made\n";
			write(ns, message, strlen(message));
			write(ns2, message, strlen(message));
			close(temp);
		}
		
	}
}
void connectHandler()
{
	char *rcvdBuff;
	char *sendBuff;
	printf("Entered Connection Handler!");
	fd_set fds;
	
	int currCli;
	
	while(1)
	{
		FD_ZERO(&fds);
		FD_SET(ns, &fds);
		FD_SET(ns2, &fds);
		
		int nfds;
		
		if(ns > ns2)
		{
			nfds = ns + 1;
		}
		else
		{
			nfds = ns2 + 1;
		}
		
		select(nfds, &fds, NULL, NULL, NULL);
	
		if(FD_ISSET(ns, &fds))
		{
			currCli = ns;
		}
		if(FD_ISSET(ns2, &fds))
		{
			currCli = ns2;
		}
		
		recv(currCli, &rcvdBuff, sizeof(rcvdBuff), 0);
		
		if(strcmp(rcvdBuff, "hELLO") == 0)
		{
			printf("HELLO RECEIVED\n");
		}
	}*/
} 
