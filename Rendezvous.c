#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>


#define BUFSIZE 1024
#define PACOTE 12000
#define PACOTEX 12001
#define PORT 9001
#define SERVER_IP "0.0.0.0"

typedef struct
{
    	unsigned int address, aux, aux2, cod;
	unsigned short  port;
	unsigned char id[3], cks[50], data[PACOTEX];
} client;


int main()
{

	int udpSocket, nBytes;
  	char buffer[BUFSIZE];
  	struct sockaddr_in serverAddr, clientAddr;
	client clients[8];
  	socklen_t addr_size, client_addr_size;
  	int i,j,k,c=0;

	udpSocket = socket(PF_INET, SOCK_DGRAM, 0);

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);	
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero); 

	
	bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

	addr_size = sizeof serverAddr;

	  while(1)
	  {
	  
		nBytes = recvfrom(udpSocket,buffer,BUFSIZE,0,(struct sockaddr *)&serverAddr, &addr_size);
		buffer[nBytes] = 0;
		
		printf("\nMessage: %s\nReceived From: %s:%d\n", buffer, inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));
		
		
		clients[c].address = serverAddr.sin_addr.s_addr;
		clients[c].port = serverAddr.sin_port;
		strcpy(clients[c].id,buffer);
		
		c++;	
				
		for(j=0;j<c;j++)	
		{
			
			serverAddr.sin_addr.s_addr = clients[j].address;
			serverAddr.sin_port = clients[j].port;

			printf("Sending %s:%d\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));

			for(k=0;k<c;k++)
			{	
				sendto(udpSocket,&clients[k],BUFSIZE,0,(struct sockaddr *)&serverAddr,addr_size);
			}
			
			
		}
		printf("Number of clients connected: %d\n", c);
	
  	}
  	return 0;
}
