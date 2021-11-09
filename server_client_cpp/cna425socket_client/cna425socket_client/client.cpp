#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <winsock2.h>

#include <ws2tcpip.h>

#include <stdlib.h>

#include <stdio.h>


//needed for inputs and outputs
using namespace std;
#include <iostream>
#include <iomanip>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib

#pragma comment (lib, "Ws2_32.lib")

#pragma comment (lib, "Mswsock.lib")

#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512

#define DEFAULT_PORT "27015"

int __cdecl main(int argc, char** argv)

{
	char message[512];
	cout << "enter what you want to send to the server: ";
	cin.getline(message, 512);
	WSADATA wsaData;

	SOCKET ConnectSocket = INVALID_SOCKET;

	struct addrinfo* result = NULL,

		* ptr = NULL,

		hints;

	

	char recvbuf[DEFAULT_BUFLEN];

	int iResult;

	int recvbuflen = DEFAULT_BUFLEN;

	// Validate the parameters

	if (argc != 2) {

		printf("usage: %s server-name\n", argv[0]);

		return 1;

	}

	// Initialize Winsock

	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0) {

		printf("WSAStartup failed with error: %d\n", iResult);

		return 1;

	}

	ZeroMemory(&hints, sizeof(hints));

	hints.ai_family = AF_UNSPEC;

	hints.ai_socktype = SOCK_STREAM;

	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port

	iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);

	if (iResult != 0) {

		printf("getaddrinfo failed with error: %d\n", iResult);

		WSACleanup();

		return 1;

	}

	// Attempt to connect to an address until one succeeds

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server

		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,

			ptr->ai_protocol);

		if (ConnectSocket == INVALID_SOCKET) {

			printf("socket failed with error: %ld\n", WSAGetLastError());

			WSACleanup();

			return 1;

		}

		// Connect to server.

		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);

		if (iResult == SOCKET_ERROR) {

			closesocket(ConnectSocket);

			ConnectSocket = INVALID_SOCKET;

			continue;

		}

		break;

	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {

		printf("Unable to connect to server!\n");

		WSACleanup();

		return 1;

	}

	// Send an initial buffer

	iResult = send(ConnectSocket, message, (int)strlen(message), 0);

	if (iResult == SOCKET_ERROR) {

		printf("send failed with error: %d\n", WSAGetLastError());

		closesocket(ConnectSocket);

		WSACleanup();

		return 1;

	}

	cout << "To be sent to server '" << message << "'\n"; 

	// shutdown the connection since no more data will be sent

	iResult = shutdown(ConnectSocket, SD_SEND);

	if (iResult == SOCKET_ERROR) {

		printf("shutdown failed with error: %d\n", WSAGetLastError());

		closesocket(ConnectSocket);

		WSACleanup();

		return 1;

	}

	// Receive until the peer closes the connection

	do {

		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		char messageBuffer1[512];
		if (iResult > 0) {
			//making a new character string of size that (probably) won't go over, shiftig in here so can put null character at end so not a bunch of gobaldygook at the end
			for (int i = 0; i < iResult; i++) //within the array...
			{
				messageBuffer1[i] = recvbuf[i]; //move value over one at a time
			}
			messageBuffer1[iResult] = '\0'; //append null value at end so knows it is the end of the message
			printf("Echoed message: '"); //print out
			printf(messageBuffer1); //print out, prints out an integer instead of full message if all on same print statement for some reason
			printf("'\n"); //print out
			
		}
		else if (iResult == 0)

			printf("Connection closed\n");

		else

			printf("recv failed with error: %d\n", WSAGetLastError());

	} while (iResult > 0);

	// cleanup

	closesocket(ConnectSocket);

	WSACleanup();

	return 0;

}