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
