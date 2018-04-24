// ***********************************************************
// * Matthew Floyd                                           *
// * 3600.01                                                 *
// * Due: 04/13/2018                                         *
// *                                                         *
// * This program sets up a server to be used in a Ponzi     *
// * scheme                                                  *
// ***********************************************************

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <netinet/in.h>

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
    int cryptoCurrency = 30;
    int balance = 0;
    int randAmount;
    char sendAmount[1024] = {};
    time_t t;
    srand((unsigned) time(&t));
    int i;
    int count = 40;
    
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
    clilen2 = sizeof(cli_addr2);
    /* Accept another connection. */
    if ((ns2 = accept(s, (struct sockaddr *) &cli_addr2, &clilen2)) < 0)
    {
        perror("accept");
        exit(1);
    }
    
    maxfd = (ns > ns2 ? ns : ns2) + 1;
    while (count != 0)
    {
        /* Set up polling using select. */
        FD_ZERO(&fds);
        FD_SET(ns,&fds);
        FD_SET(ns2,&fds);
        
        /* Wait for some input. */
        nready = select(maxfd, &fds, (fd_set *) 0, (fd_set *) 0,
                        (struct timeval *) 0);
        /* If either descriptor has some input,
         read it and copy it to the other. */
        for (i = 0; i < 1023; i++)
		buf[i] = '\0';

        if( FD_ISSET(ns, &fds))
        {
            nread = recv(ns, buf, sizeof(buf), 0);
            /* If error or eof, terminate. */
            if(nread < 1)
            {
                close(ns);
                close(ns2);
            } 
            if (strcmp(buf, "BUY") == 0) {
                if (cryptoCurrency > 0) {
                    randAmount = rand() % (5000 + 1 - 3000) + 3000;
		    sprintf(sendAmount, "%d", randAmount);
                    send(ns, sendAmount, nread + 10, 0);
                    balance += randAmount;
                    cryptoCurrency--;
		    count--;
		    printf("[CLIENT 1]: %s\n", buf);
		    printf("[SERVER X]: Client 1 %s $%d.00\n", buf, randAmount);
                }
                else
                    send(ns, "0", nread + 10, 0); // no sale
            }
            else if (strcmp(buf, "SELL") == 0) {
                randAmount = rand() % (5000 + 1 - 3000) + 3000;
                if (balance >= randAmount) {
		    sprintf(sendAmount, "%d", randAmount);
                    send(ns, sendAmount, nread + 10, 0);
                    balance -= randAmount;
                    cryptoCurrency++;
		    count--;
		    printf("[CLIENT 1]: %s\n", buf);
		    printf("[SERVER X]: Client 1 %s $%d.00\n", buf, randAmount);
                }
                else
                    send(ns, "0", nread + 10, 0); // no sale
            }
        }

         for (i = 0; i < 1023; i++)
		buf[i] = '\0';


        if( FD_ISSET(ns2, &fds))
        {
            nread = recv(ns2, buf, sizeof(buf), 0);
            /* If error or eof, terminate. */
            if(nread < 1)
            {
                close(ns);
                close(ns2);
            }
            if (strcmp(buf, "BUY") == 0) {
                if (cryptoCurrency > 0) {
                    randAmount = rand() % (5000 + 1 - 3000) + 3000;
		    sprintf(sendAmount, "%d", randAmount);
                    send(ns, sendAmount, nread + 10, 0);
                    send(ns2, sendAmount, nread + 10, 0);
                    balance += randAmount;
                    cryptoCurrency--;
		    count--;
		    printf("[CLIENT 2]: %s\n", buf);
		    printf("[SERVER X]: Client 2 %s $%d.00\n", buf, randAmount);
                }
                else
                    send(ns2, "0", nread + 10, 0); // no sale
            }
            else if (strcmp(buf, "SELL") == 0) {
                randAmount = rand() % (5000 + 1 - 3000) + 3000;
                if (balance >= randAmount) {
		    sprintf(sendAmount, "%d", randAmount);
                    send(ns, sendAmount, nread + 10, 0);
                    send(ns2, sendAmount, nread + 10, 0);
                    balance -= randAmount;
                    cryptoCurrency++;
		    count--;
		    printf("[CLIENT 2]: %s\n", buf);
		    printf("[SERVER X]: Client 2 %s $%d.00\n", buf, randAmount);
                }
                else
                    send(ns2, "0", nread + 10, 0); // no sale
            }
        }
    }
    close(ns);
    close(ns2);
    printf("\n[SERVER X]: CRYPTO %d units   BALANCE: $%d.00\n", cryptoCurrency, balance);
}



