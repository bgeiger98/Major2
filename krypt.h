#include <stdlib.h>
#include <stdio.h>

void crypt(char *message, int n, int e){
	char enc[128];
	int i, j;
	
		unsigned int c, m;
		for(i=0; i<strlen(message); i++){
			m=message[i];
			c=m;
			for(j=0; j<(e-1); j++){
				c = (c*m) % n;
			}
			message[i] = c;
		}
		message[i]='\0';
		
		printf("enc= %s\n", message);

	return;
}


int gcd(int a, int b){
	int tmp;
	while (1){
		tmp = a % b;
		if (tmp == 0){
			return b;
		}
		a = b;
		b = tmp;
	}
	return 1;
}

void keyGen(int a, int b, int *n, int *e, int *d){
	int key[4];
	
	int p, q, phi;
	int tmp, tmp2;
	p = a;
	q = b;
	
	(*n)= p*q;
	tmp = p-1;
	tmp2 = q-1;
	phi = tmp*tmp2;

	(*e)=2;
	while((*e)<phi){
		if(gcd(*e, phi) == 1){
			break;
		}
		else{
			( *e )++;
		}
		
	}
	
	int i = 0;
	int j;
	while(1){
		tmp = (phi*i) +1;
		if((tmp % ( *e )) == 0){
			*d = tmp /( *e );
			break;
		}
		i++;
	}

	return;
}


Benjamin Geiger
Jobert Cabuslay
Matthew Floyd
Michael Wilson

-----------------------------------------
Organization
-----------------------------------------

Michael handled the encryption/decryption functions and
placed them into a header file. He also helped eliminate bugs.

Matthew handled client and server connectivity. He also
worked with the threading.

Jobert did threading and connectivity. He also helped
fix bugs.

Ben laid out design on paper, helped crunch bugs, and also
wrote this READ ME file.

-----------------------------------------
Design
-----------------------------------------

The entire encryption setup is housed within a header
file called krypt.h. the crypt() function is the encryption
and decryption algorithm given in the instructions. gcd()
finds a greatest common denominator from two integers.
keyGen() generates a key based on provided values.

As for the server, it begins by setting up a listening socket.
It binds the socket then begins listening for incoming connections.
The server accepts two client connections. It sends an error if
more than two try and connect. After establishing these connections,
a select() function waits for input from one of the two clients.
Upon receiving the values from one of them, public and private keys
are generated. The private key is sent back to the client that sent
the prime numbers. The public key is sent to the other client. The
server repeats this process. If a client disconnects, the server
will accept new clients.

The client opens its own socket and connects to the server. It then
uses a select() to wait for either user input or a message received
from server. If the user enters the two prime numbers, it sends the
values to the server. Upon receiving a primary key, it creates a
pthread that creates a "sub-server" that handles connection to the
other client. It then connects to the other client and sends an 
encrypted message to the other client using the private key. If the
client receives the public key before user input, it creates a pthread
that creates a "sub-client" that handles connection to the other
client's sub-server. It connects to the other client and recieves the
encrypted message. The client uses the public key to decrypt the
message. After both these tasks are complete, the threads are joined
on their respective clients and thehy revert back to their select()
state to prepare for the next input.

-----------------------------------------
Specification
-----------------------------------------

If prime numbers aren't entered, it asks the user for values again.
The program will not run unless the correct number of command line
arguments are entered.

-----------------------------------------
Bugs
-----------------------------------------

No known bugs.
