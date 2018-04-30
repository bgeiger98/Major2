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

void *connectHandler(void *sock_desc);

int main(int argc, char *argv[])
{
    char buf[1024];                /* Buffer for messages to others. */
    char msg[1024];
    int s;                          /* Listen socket */
    int ns;                         /* Socket for first connection. */
    int ns2;                        /* Socket for second connection. */
    int len;                        /* len of sockaddr */
                    /* # descriptors ready. */
    int portno;
    struct sockaddr_in serv_addr, cli_addr1, cli_addr2;
    socklen_t clilen1, clilen2;

    int i;
	
	pthread_t tid;
    
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
	else
	{
		if(pthread_create(&tid, NULL, connectHandler, (void*) &ns) < 0)
		{
			perror("thread");
			exit(1);
		}
	}
	
	pthread_join(tid, NULL);
	
    clilen2 = sizeof(cli_addr2);
    /* Accept another connection. */
    if ((ns2 = accept(s, (struct sockaddr *) &cli_addr2, &clilen2)) < 0)
    {
        perror("accept");
        exit(1);
    }
	else
	{
		if(pthread_create(&tid, NULL, connectHandler, (void*) &ns) < 0)
		{
			perror("thread");
			exit(1);
		}
	}
}

void *connectHandler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[2000];
     
    //Send some messages to the client
    message = "Greetings! I am your connection handler\n";
    write(sock , message , strlen(message));
     
    message = "Now type something and i shall repeat what you type \n";
    write(sock , message , strlen(message));
     
    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        //end of string marker
		client_message[read_size] = '\0';
		
		//Send the message back to client
        write(sock , client_message , strlen(client_message));
		
		//clear the message buffer
		memset(client_message, 0, 2000);
    }
     
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
    close(sock);
    return 0;
} 
