#define WIN32_LEAN_AND_MEAN
#include<winsock2.h>
#include<stdio.h>
#include<string>
#include<iostream>
#include<process.h> // using thread
#pragma comment (lib, "Ws2_32.lib")
#define BUFFER 1024
bool echo = false;
CRITICAL_SECTION cs;

unsigned int WINAPI fn1(void* p) {
	
	//반환형 , 호출규약, 함수
	char recvbuf[BUFFER];
	int recvbuflen = BUFFER;
	SOCKET Client = (SOCKET) p;
	printf("thread start!\n");
	
	while(recv(Client, recvbuf, recvbuflen,0) > 0)
	{
		EnterCriticalSection(&cs);
		printf("%s\n",recvbuf);
		if(echo == TRUE)
		{
			if(send(Client, recvbuf, recvbuflen, 0) == SOCKET_ERROR)
			{
				printf("ERROR : the buffer back to the sender\n");
				closesocket(Client);
				WSACleanup();
				exit(0);
			}
		
		}
		LeaveCriticalSection(&cs);
	}
	return 0;
}


void resetAddress(sockaddr_in *serverAddr, ADDRESS_FAMILY sin_family, int port, ULONG sin_addr)
{
	ZeroMemory(serverAddr, sizeof(*serverAddr));
	serverAddr->sin_family = sin_family;
	serverAddr->sin_port = htons(port); //chage network byte
	serverAddr->sin_addr.s_addr = sin_addr;
}


int main(int argc, char **argv)
{
	
	DWORD TIME = 1;
	WSADATA wsaData;
	SOCKET Listen = INVALID_SOCKET;
	SOCKET Client = INVALID_SOCKET;
	struct sockaddr_in serverAddr;
	int port = 0;
	InitializeCriticalSection(&cs);//criticalsection reset

	if(!(argc <= 3 && argc >=2))
	{
		printf("syntax : netserver <port> [-echo]\n");
		exit(0);
	}
	if(argc == 3 && strcmp(argv[2],"-echo") == 0)
		{
		    echo = true;
		}
	printf("Hello Netserver!!\n");
	port = atoi(argv[1]);

	if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0) //version
	{
		printf("Error : Initialize Winsock\n");
		WSACleanup();
		exit(0);
	}

	if((Listen = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("ERROR : Create a Socket for connetcting to server\n");
		WSACleanup();
		exit(0);
	}
	//server reset
	resetAddress(&serverAddr, AF_INET, port, htonl(INADDR_ANY));

	//TCP read
	if(bind(Listen, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) != 0)
	{
		printf("ERROR : Setup the TCP Listening socket\n");
		closesocket(Listen);
		WSACleanup();
		exit(0);
	}

	//listen
	if(listen(Listen, SOMAXCONN) == SOCKET_ERROR)
	{
		printf("ERROR : Listen\n");
		closesocket(Listen);
		WSACleanup();
		exit(0);
	}
	else
		printf("Listening.....\n");

while(1)
{

	while((Client = accept(Listen, NULL, NULL)) != INVALID_SOCKET)
	{ 
		printf("Connetcted\n");	
		_beginthreadex(NULL,0, fn1,(void*)Client,0,NULL);

	}	
	
		DeleteCriticalSection(&cs);
}

closesocket(Listen);
closesocket(Client);
WSACleanup();
return 0;
}