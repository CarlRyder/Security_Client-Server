// Made by Y.Sendov. March 2022 

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_ERROR -1
#define DEFAULT_BUFLEN 256
#define DEFAULT_PORT "27015" // Port number

int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	printf("The server. Waiting for connection to the client...\n\n");

	// Initializing Winsock
	WSADATA wsaData;

	// Initializing the Windows Sockets interface
	int Result;
	Result = WSAStartup(MAKEWORD(1, 1), &wsaData);
	if (Result != 0)
	{
		printf("Winsock initialization error!\n");
		return DEFAULT_ERROR;
	}

	// Socket formation (addrinfo is a structure used to store information about the host address)
	struct addrinfo* result = NULL; // Preparing the socket address structure
	struct addrinfo hints; // Socket Type Information
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_flags = AI_PASSIVE; // Assign the host address to the socket (the socket address will be used when calling the binding function)
	hints.ai_family = AF_INET; // Specifying the protocol version (IPv4)
	hints.ai_socktype = SOCK_STREAM; // Provides consistent, reliable, two-way byte streams based on connections with a data transmission mechanism
	hints.ai_protocol = IPPROTO_TCP; // TCP is one of the main protocols designed to control data transmission

	// Converting a hostname to an address
	Result = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (Result != 0)
	{
		printf("Error! The getaddrinfo function failed.\n");
		WSACleanup();
		return DEFAULT_ERROR;
	}

	// Configuring a socket to listen for client connection requests
	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol); // Domain, socket type specification, protocol
	if (ListenSocket == INVALID_SOCKET)
	{
		printf("Error! Failed to create a socket.\n");
		freeaddrinfo(result);
		WSACleanup();
		return DEFAULT_ERROR;
	}

	// Binding a local address to a socket
	Result = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen); // Address, address length
	if (Result == SOCKET_ERROR)
	{
		printf("Error binding the local address to the socket!\n");
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return DEFAULT_ERROR;
	}
	freeaddrinfo(result);

	// Listening to the IP address and port for incoming connection requests
	Result = listen(ListenSocket, SOMAXCONN); // SOMAXCONN - the maximum reasonable value for a backlog
	if (Result == SOCKET_ERROR) 
	{
		printf("Error listening for incoming connections!\n");
		closesocket(ListenSocket);
		WSACleanup();
		return DEFAULT_ERROR;
	}

	// Accepting a client socket (accepting a connection to a socket)
	SOCKET ClientSocket = INVALID_SOCKET;
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET)
	{
		printf("Error allowing incoming connection attempt!\n");
		closesocket(ListenSocket);
		WSACleanup();
		return DEFAULT_ERROR;
	}

	printf("The server is successfully connected to the client!\nReady to delete the file from the client's computer.\nEnter the path to the file: ");
	char way[DEFAULT_BUFLEN];
	fgets(way, sizeof(way), stdin);
	fseek(stdin, 0, SEEK_END);
	way[strcspn(way, "\n")] = 0;

	Result = send(ClientSocket, way, (int)strlen(way), 0); // Sending data to a connected client socket
	if (Result == SOCKET_ERROR)
	{
		printf("Error! Failed to send data to client.\n");
		closesocket(ClientSocket);
		WSACleanup();
	}
	memset(way, 0, sizeof(way));

	Result = recv(ClientSocket, way, DEFAULT_BUFLEN, 0); // Getting data from a connected client socket
	printf("%s\n", way);
	return 0;
}
