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
#include <openssl/md5.h>

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
	unsigned char id[3], cks[10], data[10];
} espelho;

typedef struct
{
    	unsigned int address, aux, aux2, cod;
	unsigned short  port;
	unsigned char id[3], cks[50], data[PACOTEX];
} client;

int main(int argc, char* argv [])
{
	srand(time(NULL));
	int clientSocket, portNum, nBytes, flag, flag2=0, c=0, i, r, y=0,z=0,code;
	char msg[10], rmsg[512], buffer2[256], filename[50];
	struct sockaddr_in clientAddr, serverAddr;
	client buffer;
	client server;
  	socklen_t addr_size;

	espelho *ack;

	ack = (espelho*) malloc (44625 * sizeof(espelho));

	struct timeval timeout;      
    	timeout.tv_sec = 10;
    	timeout.tv_usec = 0;

	FILE *fp;
	fp = fopen(argv[1],"wb");

	client *clients;
	fdata *filedata;
		
	clients = (client*) malloc (44625 * sizeof(client));	
	filedata = (fdata*) malloc (44625 * sizeof(fdata));
	
	clientSocket = socket(PF_INET, SOCK_DGRAM, 0);
	
	size_t rbytes, wBytes = 0;
	long Filesize, Sizecheck = 0, Sizecheck2 = 0;

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
	
	code=0;
	while(1)
	{
	    	
		recvfrom(clientSocket,&buffer,sizeof(buffer),0,(struct sockaddr *)&serverAddr,&addr_size);		
						
		printf("Received from: %s:%d\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));

		if (setsockopt (clientSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
		{
			strcpy(buffer.cks,"timeout");
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
					
					sendto(clientSocket,"SYN",3,0, (struct sockaddr*)&serverAddr, addr_size);				
					
				}
				else
				{
					break;
				}
							
				
			}
		}
		else 
				if(strcmp(buffer.data, "Enviado") == 0)
				{
					Filesize = buffer.aux;
					Sizecheck2 = Filesize;		
					while(Sizecheck < Filesize)
					{	
						
						int len = buffer.aux2;
						
						if(Sizecheck2 == len)
						{
							wBytes = fwrite(filedata[z].fbuffer, 1, len, fp);
							Sizecheck += len;
							printf("Escrita finalizada\n");
							free(clients);
							free(filedata);	
							fclose(fp);			
						}
						
						else
						{
							wBytes = fwrite(filedata[z].fbuffer, 1, PACOTE, fp);
							Sizecheck += PACOTE;
							Sizecheck2 -=PACOTE;
						}
						z++;
							
					}
					break;
				}				
				
				else
				{
					char *output = str2md5(buffer.data, PACOTEX);
					clients[i].cod = code;
					printf("Codigo recebido:[%d]\n", buffer.cod);
					if(buffer.cod < code)
					{	
						y--;
						printf("Pacote[%d] Repetido\n", y);
						y++;
						sendto(clientSocket,&ack[i],sizeof(ack[i]),0,(struct sockaddr*)&serverAddr, addr_size);
						
					}

					else if(strcmp(buffer.cks, output) == 0)
					{	
						code++;
						ack[i].cod = code;
						free(output);
						copystuff(filedata[y].fbuffer, buffer.data);
						printf("Pacote[%d] Aceito\n", y);						
						y++;
						sendto(clientSocket,&ack[i],sizeof(ack[i]),0,(struct sockaddr*)&serverAddr, addr_size);
						
						
					}
					else
					{
						y++;
						printf("Codigo Enviado[%d]\n", clients[i].cod);
						sendto(clientSocket,&ack[i],sizeof(ack[i]),0,(struct sockaddr*)&serverAddr, addr_size);
						printf("Pacote[%d] Negado\n", y);
						y--;
					}

				}		

		}
		
	}
		
	
  return 0;
}
