#define WIN32_LEAN_AND_MEAN
#include <winsock2.h> 
#include <ws2tcpip.h> 
#include <stdio.h>
#include <string>
#include <string.h>
#include <iostream> 
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512

void validateParams(int argc, char **argv, char **addr, int *port)
{
	if(argc != 3) {
			printf("syntax : netclient <ip> <port>\n");
			printf("exam : netclient 127.0.0.1 9999\n");
			exit(0);
	}
	 *addr = argv[1];
	 *port = atoi(argv[2]);

} //인자값 확인

void setSockAddrIn(sockaddr_in *serverAddr, ADDRESS_FAMILY sin_family, int port, char *addr)
{
	ZeroMemory(serverAddr, sizeof(*serverAddr));
	serverAddr->sin_family = sin_family;
	serverAddr->sin_port = htons(port);
	inet_pton(AF_INET, addr, &(serverAddr->sin_addr));
}

int main(int argc, char **argv)
{
	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
		
	DWORD recvTimeout= 60;

	struct sockaddr_in serverAddr;

	char sendbuf[DEFAULT_BUFLEN];
	int sendbuflen = DEFAULT_BUFLEN;

	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	char *addr;
	int port;

	validateParams(argc, argv, &addr, &port);

	if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
	{
		printf("Error : Initialize Winsock.\n");
		exit(0);
	} //원속 버전 확인

	if((ConnectSocket = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("ERROR : Create a Socket for conneting to server");
		WSACleanup();
		exit(0);
	}

	setSockAddrIn(&serverAddr, AF_INET, port, addr);

	//연결
	if(connect(ConnectSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		printf("Error : Connect to server\n");
		closesocket(ConnectSocket);
		WSACleanup();
		exit(0);
	}

	setsockopt(ConnectSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&recvTimeout, sizeof(recvTimeout));

	while(1) 
	{
		printf("please enter the message\n");
	
		scanf("%s",sendbuf);


		if(strcmp(sendbuf, "quit") ==  0) 
		{
			break;
		}
		if(send(ConnectSocket, sendbuf, sendbuflen, 0) == SOCKET_ERROR)
		{
			printf("Error : Send an initial buffer\n");
			closesocket(ConnectSocket);
			WSACleanup();
			exit(0);
		}

		if(recv(ConnectSocket, recvbuf, recvbuflen, 0) > 0)
		{
			printf("received data\n %s \n",recvbuf);
		}
	}
	closesocket(ConnectSocket);
	WSACleanup();
	return 0;

}