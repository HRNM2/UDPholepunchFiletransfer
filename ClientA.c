#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <arpa/inet.h>
#include <time.h>
#include <fcntl.h>
#include <openssl/md5.h>
#include <errno.h>

#define BUFSIZE 1024
#define PACOTE 12000
#define PACOTEX 12001
#define PORT 9001
#define SERVER_IP "127.0.0.1"

void copystuff(char *skt, char *skt2)
{	
    	int h;
	for (h = 0; h < PACOTE; h++) 
	{
      		skt[h] = skt2[h];
   	}

}

char *str2md5(const char *str, int length) {
    int n;
    MD5_CTX c;
    unsigned char digest[16];
    char *out = (char*)malloc(33);

    MD5_Init(&c);

    while (length > 0) {
        if (length > 512) {
            MD5_Update(&c, str, 512);
        } else {
            MD5_Update(&c, str, length);
        }
        length -= 512;
        str += 512;
    }

    MD5_Final(digest, &c);

    for (n = 0; n < 16; ++n) {
        snprintf(&(out[n*2]), 16*2, "%02x", (unsigned int)digest[n]);
    }

    return out;
}

typedef struct
{
	char fbuffer[PACOTEX];
} fdata;

typedef struct
{
    	unsigned int address, aux, aux2, cod;
	unsigned short  port;
	unsigned char id[3], cks[50], data[PACOTEX];
} client;

int main(int argc, char* argv [])
{
	srand(time(NULL));
	int clientSocket, portNum, nBytes, flag, flag2, c=0, i, r, y=0, code;
	int PacotesY=0, PacotesN=0;
	char msg[512], sid[50], size[10], filename[50];
	unsigned char rdata[256];

	struct sockaddr_in clientAddr, serverAddr;
	client buffer;
	client server;
	
	struct timeval timeout;      
    	timeout.tv_sec = 3;
    	timeout.tv_usec = 0;

	client *clients;
	fdata *filedata;

	FILE *fh = fopen(argv[1],"rb");
	if(fh == NULL)
	{
		perror("fopen");
		exit(1);
	}
 	fseek(fh, 0, SEEK_END);	
	unsigned long Filesize = (unsigned long)ftell(fh);
	rewind(fh);

	clients = (client*) malloc (44625 * sizeof(client));	
	filedata = (fdata*) malloc (44625 * sizeof(fdata));
		
	size_t rbytes, sentBytes = 0;
	long Sizecheck = 0;

	char *output;	

	socklen_t addr_size;

	clientSocket = socket(PF_INET, SOCK_DGRAM, 0);

	do
	{
		rbytes = fread(filedata[y].fbuffer, 1, PACOTE, fh);
		y++;

	}while(rbytes == PACOTE);

	fclose(fh);
	clientAddr.sin_family = AF_INET;
    	clientAddr.sin_port = htons(PORT);
	clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(&clientAddr.sin_zero, '\0', sizeof(clientAddr.sin_zero));	

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

	addr_size = sizeof serverAddr;

	server.address = serverAddr.sin_addr.s_addr;
	server.port = serverAddr.sin_port;

	r = rand() % (999 + 1 -0) + 0;
	sprintf(msg,"%d",r);
	sendto(clientSocket,msg,3,0, (struct sockaddr*)&serverAddr, addr_size);
	code = 1;
	while(1)
	{
	    	
		recvfrom(clientSocket,&buffer,sizeof(buffer),0,(struct sockaddr *)&serverAddr,&addr_size);		
				
		printf("Received from: %s:%d\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));
		
		if (setsockopt (clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
		{
			
		}       

		if (server.address == serverAddr.sin_addr.s_addr && server.port == (serverAddr.sin_port))
		{

			flag = 0;

			for(i=0; i < c && flag == 0; i++)
			{
				if(clients[i].address == buffer.address && clients[i].port == buffer.port)
				{
					flag = 1;
				}				
				
			}

			if(flag == 0)
			{
				
				clients[c].address = buffer.address;
				clients[c].port = buffer.port;
				strcpy(clients[c].id,buffer.id);
				c++;

				serverAddr.sin_addr.s_addr = buffer.address;
            			serverAddr.sin_port = buffer.port;
				printf("New Client[%s] %s:%d\n", buffer.id, inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));
			}
            		
			printf("Number of clients connected: %d\n", c);
			
			for(i=0; i < c; i++)
			{
				
				if(strcmp(clients[i].id, msg) != 0) 
				{
					serverAddr.sin_addr.s_addr = clients[i].address;
            				serverAddr.sin_port = clients[i].port;
					
					clients[i].cod = code;
					copystuff(clients[i].data, filedata[0].fbuffer);
					output = str2md5(clients[i].data, PACOTEX);			
					strcpy(clients[i].cks,output);	
					free(output);			
					sentBytes = sendto(clientSocket,&clients[i],sizeof(clients[i]),0,(struct sockaddr*)&serverAddr, addr_size);
					Sizecheck += PACOTE;
					y=1;

					
				}
				else
				{
					break;
				}
							
				
			}
		}
		else
		{       printf("Codigo recebido:[%d]\n", buffer.cod);
			if(strcmp(buffer.data,"Recebendo") == 0)
			{	
				copystuff(clients[i].data, filedata[0].fbuffer);
				output = str2md5(clients[i].data, PACOTEX);			
				strcpy(clients[i].cks,output);	
				free(output);			
				sentBytes = sendto(clientSocket,&clients[i],sizeof(clients[i]),0,(struct sockaddr*)&serverAddr, addr_size);
				Sizecheck += PACOTE;
				y=1;
				
			}
			

			else if(buffer.cod == code)
			{	
				if(Sizecheck < Filesize)
				{	
					clients[i].cod = code;
					code++;
					copystuff(clients[i].data, filedata[y].fbuffer);
					output = str2md5(clients[i].data, PACOTEX);			
					strcpy(clients[i].cks,output);	
					free(output);			
					sentBytes = sendto(clientSocket,&clients[i],sizeof(clients[i]),0,(struct sockaddr*)&serverAddr, addr_size);
					Sizecheck += PACOTE;
					printf("Pacote[%d] Enviado\n", y);
					y++;					
					PacotesY++;
				}

				else
				{
					clients[i].aux = Filesize;
					clients[i].aux2 = rbytes;					
					strcpy(clients[i].data, "Enviado");
					sentBytes = sendto(clientSocket,&clients[i],sizeof(clients[i]),0,(struct sockaddr*)&serverAddr, addr_size);
					printf("[%d]/[%d]", PacotesY, PacotesN);
					printf("Envio Finalizado\n");
					free(clients);
					free(filedata);
					break;
		
				
				}
				
			}
			else
			{	
					
				y--;
				Sizecheck -= PACOTE;
				copystuff(clients[i].data, filedata[y].fbuffer);
				output = str2md5(clients[i].data, PACOTEX);			
				strcpy(clients[i].cks,output);	
				free(output);
				printf("Codigo enviado: [%d]\n",clients[i].cod);		
				sentBytes = sendto(clientSocket,&clients[i],sizeof(clients[i]),0,(struct sockaddr*)&serverAddr, addr_size);
				printf("Pacote[%d] Reenviado\n", y);
				y++;
				PacotesY--;
				PacotesN++;
			}
						
			
		}

	}
		
  	return 0;
}
