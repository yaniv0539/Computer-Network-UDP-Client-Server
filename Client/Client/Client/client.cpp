#define _CRT_SECURE_NO_WARNINGS		
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h> 
#include <string.h>

#define TIME_PORT	27015

void main()
{

	// Initialize Winsock (Windows Sockets).

	WSAData wsaData;
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Time Client: Error at WSAStartup()\n";
		return;
	}

	// Client side:
	// Create a socket and connect to an internet address.

	SOCKET connSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == connSocket)
	{
		cout << "Time Client: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	// For a client to communicate on a network, it must connect to a server.

	// Need to assemble the required data for connection in sockaddr structure.

	// Create a sockaddr_in object called server. 
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	server.sin_port = htons(TIME_PORT);

	// Send and receive data.

	int bytesSent = 0;
	int bytesRecv = 0;
	char sendBuff[255];
	char recvBuff[255];
	bool stop = false;
	int sendBuffInt;

	// Asks the server what's the currnet time.
	// The send function sends data on a connected socket.
	// The buffer to be sent and its size are needed.
	// The fourth argument is an idicator specifying the way in which the call is made (0 for default).
	// The two last arguments hold the details of the server to communicate with. 
	// NOTE: the last argument should always be the actual size of the client's data-structure (i.e. sizeof(sockaddr)).

	while (!stop)
	{
		cout << "Menu:" << endl
			<< "1. Get time" << endl
			<< "2. Get time without date" << endl
			<< "3. Get time since epoch" << endl
			<< "4. Get client to server delay estimation" << endl
			<< "5. Measure RTT" << endl
			<< "6. Get time without date or seconds" << endl
			<< "7. Get year" << endl
			<< "8. Get month and day" << endl
			<< "9. Get seconds since begining of month" << endl
			<< "10. Get week of year" << endl
			<< "11. Get day light savings" << endl
			<< "12. Get time without date in city" << endl
			<< "13. Measure time lap" << endl
			<< "14. Exit" << endl;

		cin >> sendBuff;

		sendBuffInt = atoi(sendBuff);
		
		if (sendBuffInt < 1 || sendBuffInt > 14)
		{
			printf("Error!!");
			break;
		}

		switch (sendBuffInt)
		{
		case 4:
		{
			double avg = 0;
			DWORD prevTick, currTick;

			for (int i = 0; i < 100; i++)
			{
				bytesSent = sendto(connSocket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&server, sizeof(server));

				if (SOCKET_ERROR == bytesSent)
				{
					cout << "Time Client: Error at sendto(): " << WSAGetLastError() << endl;
					closesocket(connSocket);
					WSACleanup();
					return;
				}
			}

			for (int i = 0; i < 100; i++)
			{
				// Gets the server's answer using simple recieve (no need to hold the server's address).
				bytesRecv = recv(connSocket, recvBuff, 255, 0);
				if (SOCKET_ERROR == bytesRecv)
				{
					cout << "Time Client: Error at recv(): " << WSAGetLastError() << endl;
					closesocket(connSocket);
					WSACleanup();
					return;
				}

				recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string

				if (i == 0)
				{
					prevTick = atoi(recvBuff);
					continue;
				}

				currTick = atoi(recvBuff);
				avg += (currTick - prevTick);
				prevTick = currTick;
			}

			avg /= 99;

			cout << "Client to server delay estimation is: " << avg << endl;
			continue;
		}

		case 5:
		{
			double avg = 0;
			DWORD sendTick[100], recvTick[100];

			for (int i = 0; i < 100; i++)
			{
				bytesSent = sendto(connSocket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&server, sizeof(server));
				
				if (SOCKET_ERROR == bytesSent)
				{
					cout << "Time Client: Error at sendto(): " << WSAGetLastError() << endl;
					closesocket(connSocket);
					WSACleanup();
					return;
				}

				sendTick[i] = GetTickCount();

				// Gets the server's answer using simple recieve (no need to hold the server's address).
				bytesRecv = recv(connSocket, recvBuff, 255, 0);
				if (SOCKET_ERROR == bytesRecv)
				{
					cout << "Time Client: Error at recv(): " << WSAGetLastError() << endl;
					closesocket(connSocket);
					WSACleanup();
					return;
				}

				recvTick[i] = GetTickCount();

				recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
			}

			for (int i = 0; i < 100; i++)
			{
				avg = recvTick[i] - sendTick[i];
			}

			avg /= 100;

			cout << "Client to server delay estimation is: " << avg << endl;
			continue;
		}

		case 12:
		{
			int userChoice;

			cout << "Please select a city:" << endl
				<< "1. Doha" << endl
				<< "2. Prague" << endl
				<< "3. New York" << endl
				<< "4. Berlin" << endl
				<< "For any other city that is not on the list and to get the universal time please select a random number." << endl;

			cin >> userChoice;

			switch (userChoice)
			{
			case 1:
				strcat(sendBuff, "1");
				break;
			case 2:
				strcat(sendBuff, "2");
				break;
			case 3:
				strcat(sendBuff, "3");
				break;
			case 4:
				strcat(sendBuff, "4");
				break;
			default:
				strcat(sendBuff, "0");
				break;
			}
			break;
		}

		case 14:
			stop = true;
			break;

		default:
			break;
		}

		if (stop)
		{
			break;
		}

		bytesSent = sendto(connSocket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&server, sizeof(server));

		if (SOCKET_ERROR == bytesSent)
		{
			cout << "Time Client: Error at sendto(): " << WSAGetLastError() << endl;
			closesocket(connSocket);
			WSACleanup();
			return;
		}
		cout << "Time Client: Sent: " << bytesSent << "/" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";

		// Gets the server's answer using simple recieve (no need to hold the server's address).
		bytesRecv = recv(connSocket, recvBuff, 255, 0);
		if (SOCKET_ERROR == bytesRecv)
		{
			cout << "Time Client: Error at recv(): " << WSAGetLastError() << endl;
			closesocket(connSocket);
			WSACleanup();
			return;
		}

		recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
		cout << "Time Client: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message.\n";
	}

	// Closing connections and Winsock.
	cout << "Time Client: Closing Connection.\n";
	closesocket(connSocket);

	system("pause");
}