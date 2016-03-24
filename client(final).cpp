#define WIN32_LEAN_AND_MEAN
#include <winsock2.h> 
#include <ws2tcpip.h> 
#include <stdio.h>
#include <string>
#include <string.h>
#include <iostream> 
#pragma comment (lib, "Ws2_32.lib")
#include <process.h>
#define BUFFER 1024

unsigned int WINAPI fn1(void *p)
{
	char recvbuf[BUFFER];
	int recvbuflen = BUFFER;
	SOCKET ConnectSocket = (SOCKET)p;
	if(recv(ConnectSocket, recvbuf, recvbuflen, 0) > 0)
	{
		printf("echo : %s",recvbuf);
		printf("\n");
	}	return 0;
}//socket rcv funtion 입력한 버퍼를 받는 thread


void SetAddress(sockaddr_in *serverAddr, ADDRESS_FAMILY sin_family, int port, char *addr)
{
	ZeroMemory(serverAddr, sizeof(*serverAddr));
	serverAddr->sin_family = sin_family;
	serverAddr->sin_port = htons(port);
	serverAddr->sin_addr.s_addr = inet_addr(addr);
}

int main(int argc, char **argv)
{
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct sockaddr_in serverAddr;
	char sendbuf[BUFFER];
	int sendbuflen = BUFFER;
	char *addr = 0;
	int port= 0;
	if(argc != 3) {
			printf("syntax : netclient <ip> <port>\n");
			printf("exam : netclient 127.0.0.1 9999\n");
			exit(0);
	}
	addr = argv[1];
	port = atoi(argv[2]);

	if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
	{
		printf("Error : Initialize Winsock.\n");
		exit(0);
	} //version impormation

	if((ConnectSocket = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("ERROR : Create a Socket for conneting to server");
		WSACleanup();
		exit(0);
	}//create socket

	SetAddress(&serverAddr, AF_INET, port, addr);

	//connect
	if(connect(ConnectSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) //SCOKET_ERROR return -1
	{
		printf("Error : Connect to server\n");
		closesocket(ConnectSocket);
		WSACleanup();
		exit(0);
	}
	
	
	
	while(1) 
	{
		_beginthreadex(NULL,0,fn1,(void*)ConnectSocket,0,NULL);	
		printf("please enter the message\n");
		scanf_s("%s",sendbuf,sizeof(sendbuf));


		if(strcmp(sendbuf, "quit") ==  0) 
		{
			break;
		}
		if(send(ConnectSocket, sendbuf, sendbuflen, 0) == SOCKET_ERROR) //SOCKET_ERROR return -1;
		{
			printf("Error : Send an initial buffer\n");
			closesocket(ConnectSocket);
			WSACleanup();
			exit(0);
		}
	

		
	}
	closesocket(ConnectSocket);
	WSACleanup();
	return 0;

}