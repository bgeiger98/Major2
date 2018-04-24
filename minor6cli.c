// ***********************************************************
// * Matthew Floyd                                           *
// * 3600.01                                                 *
// * Due: 04/13/2018                                         *
// *                                                         *
// * This program sets up a client to be used in a Ponzi     *
// * scheme                                                  *
// ***********************************************************

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
    int sockfd, portno, n, count = 0;
    int balance = 30000, cryptoCurr = 0, received = 0;
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
    
    while(count != 20) {
    printf("[CLIENT]: BALANCE = $%d\n", balance);
        if (balance > 0) {
            bzero(buffer,256);
            strcpy(buffer, "BUY");
            if ((n = write(sockfd,buffer,strlen(buffer))) < 0)
            {
                error("ERROR writing to socket");
            }
    
            if ((n = read(sockfd,buffer,255)) < 0)
            {
                error("ERROR reading from socket");
            }
            received = atoi(buffer);
            if (received == 0) {
                if (cryptoCurr > 0){
                    bzero(buffer,256);
                    strcpy(buffer, "SELL");
                    if ((n = write(sockfd,buffer,strlen(buffer))) < 0)
                    {
                        error("ERROR writing to socket");
                    }
                    if ((n = read(sockfd,buffer,255)) < 0)
                    {
                        error("ERROR reading from socket");
                    }
                    received = atoi(buffer);
                    if (received == 0) {}
                    else {
			printf("[CLIENT]: SELL\n");
			printf("[SERVER]: SELL PRICE $%d\n", received);
                        balance += received;
                        cryptoCurr--;
			count++;
                    }
                }
            }
            else {
		printf("[CLIENT]: BUY\n");
		printf("[SERVER]: BUY PRICE $%d\n", received);
                balance -= received;
                cryptoCurr++;
		count++;
            }
        }
        else {
            bzero(buffer,256);
            strcpy(buffer, "SELL");
            if ((n = write(sockfd,buffer,strlen(buffer))) < 0)
            {
                error("ERROR writing to socket");
            }
            if ((n = read(sockfd,buffer,255)) < 0)
            {
                error("ERROR reading from socket");
            }
            received = atoi(buffer);
            if (received == 0) {}
            else {
                balance += received;
		printf("[CLIENT]: SELL\n");
		printf("[SERVER]: SElL PRICE $%d\n", received);
                cryptoCurr--;
		count++;
            }
        }
        sleep(1);
    }
    printf("\n[CLIENT]: CRYPTO %d units  BALANCE: $%d.00\n", cryptoCurr, balance);
    close(sockfd); 
    return 0;
}

