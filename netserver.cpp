#define WIN32_LEAN_AND_MEAN
#include<winsock2.h>
#include<stdio.h>
#include<string>
#include<iostream>
#pragma comment (lib, "Ws2_32.lib")
#define DEFAULT_BUFLEN 512
bool Echo;
void validateParams(int argc, char **argv, int *port)
{
	if(!(argc <= 3 && argc >=2))
	{
		printf("syntax : netserver <port> [-echo]\n");
		exit(0);
	}

	if(argc == 3 && strcmp(argv[2],"-echo") == 0)
	{
		Echo = true;
	}

	*port = atoi(argv[1]);
}

void setSockAddrIn(sockaddr_in *serverAddr, ADDRESS_FAMILY sin_family, int port, ULONG sin_addr)
{
	ZeroMemory(serverAddr, sizeof(*serverAddr));
	serverAddr->sin_family = sin_family;
	serverAddr->sin_port = htons(port);
	serverAddr->sin_addr.s_addr = sin_addr;
}
UINT WINAPI ProcessClient(LPVOID IpParameter) {
	SOCKET ClientSocket = (SOCKET) IpParameter;
	
	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	while(recv(ClientSocket, recvbuf, recvbuflen,0) > 0) 
	{
		if(Echo==FALSE)
			continue;

		if(send(ClientSocket, recvbuf, recvbuflen,0) == SOCKET_ERROR)
		{
			printf("Error : Echo the buffer to the sender\n");
			closesocket(ClientSocket);
			WSACleanup();
			exit(0);
		}
		printf("Echo send\n");
	}
	 if(shutdown(ClientSocket, SD_SEND) == SOCKET_ERROR)
	{
		printf("ERROR : shutdown the connection since we are done\n");
		closesocket(ClientSocket);
		WSACleanup();
		exit(0);
	}
	else
		printf("Disconnected\n");

	closesocket(ClientSocket);
	return 0;
}

int main(int argc, char **argv)
{

	WSADATA wsaData;
	SOCKET ListenSocket = INVALID_SOCKET;
	SOCKET ClientSocket = INVALID_SOCKET;

	char recvbuf[DEFAULT_BUFLEN];
	int recvbuflen = DEFAULT_BUFLEN;

	struct sockaddr_in serverAddr;
	int port;
	printf("Wellcome to Netserver!!\n");


	validateParams(argc, argv, &port); //인자값 확인

	if(WSAStartup(MAKEWORD(2,2), &wsaData) != 0) //원속버전 확인
	{
		printf("Error : Initialize Winsock\n");
		WSACleanup();
		exit(0);
	}
	//연결소켓 생성
	if((ListenSocket = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("ERROR : Create a Socket for connetcting to server\n");
		WSACleanup();
		exit(0);
	}
	//서버 주소 입력
	setSockAddrIn(&serverAddr, AF_INET, port, htonl(INADDR_ANY));

	//서버 TCP 읽어 들이는지 확인
	if(bind(ListenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) != 0)
	{
		printf("ERROR : Setup the TCP Listening socket\n");
		closesocket(ListenSocket);
		WSACleanup();
		exit(0);
	}

	//리슨
	if(listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		printf("ERROR : Listen\n");
		closesocket(ListenSocket);
		WSACleanup();
		exit(0);
	}
	else
		printf("Listening.....\n");


while(1)
{
	while((ClientSocket = accept(ListenSocket, NULL, NULL)) == INVALID_SOCKET);
	

	printf("Connetcted\n");

		while(recv(ClientSocket, recvbuf, recvbuflen,0) > 0)
	{
		if(!Echo)
			continue;
		if(send(ClientSocket, recvbuf, recvbuflen, 0) == SOCKET_ERROR)
		{
			printf("ERROR : the buffer back to the sender\n");
			closesocket(ClientSocket);
			WSACleanup();
			exit(0);
		}
		printf("%s\n",recvbuf);
	}


}

closesocket(ListenSocket);
closesocket(ClientSocket);
WSACleanup();
return 0;
}