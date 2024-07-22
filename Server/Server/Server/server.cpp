#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <string.h>
#include <time.h>
#include "server.h"

#define TIME_PORT	27015

void main()
{
	// Initialize Winsock (Windows Sockets).

	// Create a WSADATA object called wsaData.
	// The WSADATA structure contains information about the Windows 
	// Sockets implementation.
	WSAData wsaData;

	// Call WSAStartup and return its value as an integer and check for errors.
	// The WSAStartup function initiates the use of WS2_32.DLL by a process.
	// First parameter is the version number 2.2.
	// The WSACleanup function destructs the use of WS2_32.DLL by a process.
	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Time Server: Error at WSAStartup()\n";
		return;
	}

	// Server side:
	// Create and bind a socket to an internet address.

	// After initialization, a SOCKET object is ready to be instantiated.

	// Create a SOCKET object called m_socket. 
	// For this application:	use the Internet address family (AF_INET), 
	//							datagram sockets (SOCK_DGRAM), 
	//							and the UDP/IP protocol (IPPROTO_UDP).
	SOCKET m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	// Check for errors to ensure that the socket is a valid socket.
	// Error detection is a key part of successful networking code. 
	// If the socket call fails, it returns INVALID_SOCKET. 
	// The "if" statement in the previous code is used to catch any errors that
	// may have occurred while creating the socket. WSAGetLastError returns an 
	// error number associated with the last error that occurred.
	if (INVALID_SOCKET == m_socket)
	{
		cout << "Time Server: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	// For a server to communicate on a network, it must first bind the socket to 
	// a network address.

	// Need to assemble the required data for connection in sockaddr structure.

	// Create a sockaddr_in object called serverService. 
	sockaddr_in serverService;
	// Address family (must be AF_INET - Internet address family).
	serverService.sin_family = AF_INET;
	// IP address. The sin_addr is a union (s_addr is a unsigdned long (4 bytes) data type).
	// INADDR_ANY means to listen on all interfaces.
	// inet_addr (Internet address) is used to convert a string (char *) into unsigned int.
	// inet_ntoa (Internet address) is the reverse function (converts unsigned int to char *)
	// The IP address 127.0.0.1 is the host itself, it's actually a loop-back.
	serverService.sin_addr.s_addr = INADDR_ANY;	//inet_addr("127.0.0.1");
	// IP Port. The htons (host to network - short) function converts an
	// unsigned short from host to TCP/IP network byte order (which is big-endian).
	serverService.sin_port = htons(TIME_PORT);

	// Bind the socket for client's requests.

	// The bind function establishes a connection to a specified socket.
	// The function uses the socket handler, the sockaddr structure (which
	// defines properties of the desired connection) and the length of the
	// sockaddr structure (in bytes).
	if (SOCKET_ERROR == bind(m_socket, (SOCKADDR*)&serverService, sizeof(serverService)))
	{
		cout << "Time Server: Error at bind(): " << WSAGetLastError() << endl;
		closesocket(m_socket);
		WSACleanup();
		return;
	}

	// Waits for incoming requests from clients.

	// Send and receive data.

	sockaddr client_addr;
	int client_addr_len = sizeof(client_addr);
	int bytesSent = 0;
	int bytesRecv = 0;
	char sendBuff[255];
	char recvBuff[255];
	int clientChoice;
	bool firstRequest = true;

	time_t currentTime;

	// Get client's requests and answer them.
	// The recvfrom function receives a datagram and stores the source address.
	// The buffer for data to be received and its available size are 
	// returned by recvfrom. The fourth argument is an idicator 
	// specifying the way in which the call is made (0 for default).
	// The two last arguments are optional and will hold the details of the client for further communication. 
	// NOTE: the last argument should always be the actual size of the client's data-structure (i.e. sizeof(sockaddr)).
	cout << "Time Server: Wait for clients' requests.\n";

	while (true)
	{
		bytesRecv = recvfrom(m_socket, recvBuff, 255, 0, &client_addr, &client_addr_len);
		if (SOCKET_ERROR == bytesRecv)
		{
			cout << "Time Server: Error at recvfrom(): " << WSAGetLastError() << endl;
			closesocket(m_socket);
			WSACleanup();
			return;
		}

		recvBuff[bytesRecv] = '\0'; //add the null-terminating to make it a string
		cout << "Time Server: Recieved: " << bytesRecv << " bytes of \"" << recvBuff << "\" message.\n";

		clientChoice = atoi(recvBuff);

		switch (clientChoice)
		{
		case 1:
			GetTime(sendBuff);
			break;
		case 2:
			GetTimeWithoutDate(sendBuff);
			break;
		case 3:
			GetTimeSinceEpoch(sendBuff);
			break;
		case 4:
			GetClientToServerDelayEstimation(sendBuff);
			break;
		case 5:
			MeasureRTT(sendBuff);
			break;
		case 6:
			GetTimeWithoutDateOrSeconds(sendBuff);
			break;
		case 7:
			GetYear(sendBuff);
			break;
		case 8:
			GetMonthAndDay(sendBuff);
			break;
		case 9:
			GetSecondsSinceBeginningOfMonth(sendBuff);
			break;
		case 10:
			GetWeekOfYear(sendBuff);
			break;
		case 11:
			GetDaylightSavings(sendBuff);
			break;
		case 120:
		case 121:
		case 122:
		case 123:
		case 124:
			GetTimeWithoutDateInCity(clientChoice, sendBuff);
			break;
		case 13:
			MeasureTimeLap(sendBuff, currentTime, firstRequest);
			firstRequest = false;
			break;
		default:
			break;
		}

		sendBuff[strlen(sendBuff) - 1] = '\0'; //to remove the new-line from the created string

		// Sends the answer to the client, using the client address gathered
		// by recvfrom. 
		bytesSent = sendto(m_socket, sendBuff, (int)strlen(sendBuff), 0, (const sockaddr*)&client_addr, client_addr_len);
		if (SOCKET_ERROR == bytesSent)
		{
			cout << "Time Server: Error at sendto(): " << WSAGetLastError() << endl;
			closesocket(m_socket);
			WSACleanup();
			return;
		}

		if (clientChoice == 4)
		{
			continue;
		}

		 cout << "Time Server: Sent: " << bytesSent << "\\" << strlen(sendBuff) << " bytes of \"" << sendBuff << "\" message.\n";
	}

	// Closing connections and Winsock.
	cout << "Time Server: Closing Connection.\n";
	closesocket(m_socket);
	WSACleanup();
}

// Answer client's request by the current time.
void GetTime(char* o_SendBuff)
{
	// Get the current time.
	time_t timer;
	time(&timer);

	// Parse the current time to printable string.
	strcpy(o_SendBuff, ctime(&timer));
}

// Function to get the current time without date
void GetTimeWithoutDate(char* o_SendBuff)
{
	// Get the current time
	time_t raw_time;
	time(&raw_time);

	// Convert to local time format
	struct tm* time_info;
	time_info = localtime(&raw_time);

	sprintf(o_SendBuff, "Current time: %02d:%02d:%02d\n",
		time_info->tm_hour,
		time_info->tm_min,
		time_info->tm_sec);
}

// Function to get the time since epoch (Unix timestamp)
void GetTimeSinceEpoch(char* o_SendBuff)
{
	// Get the current time
	time_t raw_time;
	time(&raw_time);

	sprintf(o_SendBuff, "%ld", (long)raw_time);
}

// Function to estimate client to server delay
void GetClientToServerDelayEstimation(char* o_SendBuff)
{
	DWORD tickCount = GetTickCount();

	sprintf(o_SendBuff, "%lu", tickCount);
}

// Function to measure round-trip time (RTT)
void MeasureRTT(char* o_SendBuff)
{
	DWORD tickCount = GetTickCount();

	sprintf(o_SendBuff, "%lu", tickCount);
}

// Function to get time without date or seconds
void GetTimeWithoutDateOrSeconds(char* o_SendBuff)
{
	// Get the current time
	time_t raw_time;
	time(&raw_time);

	// Convert to local time format
	struct tm* time_info;
	time_info = localtime(&raw_time);

	sprintf(o_SendBuff, "%02d:%02d ",
		time_info->tm_hour,
		time_info->tm_min);
}

// Function to get the current year
void GetYear(char* o_SendBuff)
{
	// Get the current time
	time_t raw_time;
	time(&raw_time);

	// Convert to local time format
	struct tm* time_info;
	time_info = localtime(&raw_time);

	sprintf(o_SendBuff, "%d ",
		time_info->tm_year + 1900);
}

// Function to get the current month and day
void GetMonthAndDay(char* o_SendBuff)
{
	// Get the current time
	time_t raw_time;
	time(&raw_time);

	// Convert to local time format
	struct tm* time_info;
	time_info = localtime(&raw_time);

	strftime(o_SendBuff, 50, "%m, %A ", time_info);
}

// Function to calculate the number of seconds that have passed since the beginning of the current month
void GetSecondsSinceBeginningOfMonth(char* o_SendBuff)
{
	// Get the current time in seconds since the Epoch
	time_t currentTime = time(NULL);

	// Get the current local time structure
	struct tm* localTime = localtime(&currentTime);

	// Create a time structure for the beginning of the month
	struct tm beginningOfMonth = *localTime;
	beginningOfMonth.tm_mday = 1;   // Set day to the 1st
	beginningOfMonth.tm_hour = 0;   // Set hour to 00
	beginningOfMonth.tm_min = 0;    // Set minute to 00
	beginningOfMonth.tm_sec = 0;    // Set second to 00

	// Convert the beginning of the month time structure to time_t
	time_t beginningOfMonthTime = mktime(&beginningOfMonth);

	// Calculate the difference in seconds between the current time and the beginning of the month
	long secondsSinceBeginningOfMonth = difftime(currentTime, beginningOfMonthTime);

	// Format the result as a string and store it in the output buffer
	sprintf(o_SendBuff, "%ld ", secondsSinceBeginningOfMonth);
}

// Function to calculate the current week number of the year
void GetWeekOfYear(char* o_SendBuff)
{
	// Get the current time in seconds since the Epoch
	time_t currentTime = time(NULL);

	// Get the current local time structure
	struct tm* localTime = localtime(&currentTime);

	// Get the day of the year (0-365)
	int dayOfYear = localTime->tm_yday + 1; // tm_yday is zero-based, so add 1

	// Get the day of the week (0-6, Sunday = 0)
	int dayOfWeek = localTime->tm_wday;

	// Calculate the current week of the year
	int weekOfYear = (dayOfYear + 6 - dayOfWeek) / 7;

	// Format the result as a string and store it in the output buffer
	sprintf(o_SendBuff, "%d ", weekOfYear);
}

// Function to check if daylight savings time is in effect
void GetDaylightSavings(char* o_SendBuff)
{
	// Get the current time
	time_t currentTime = time(NULL);

	// Get the current local time structure
	struct tm* localTime = localtime(&currentTime);

	bool isDaylightSavings = (bool)localTime->tm_isdst;

	sprintf(o_SendBuff, "%d ", isDaylightSavings);
}

// Function to get time without date in a specific city (based on client choice)
void GetTimeWithoutDateInCity(int clientChoice, char* o_SendBuff)
{
	// Get the current time
	time_t currentTime = time(NULL);

	struct tm* utcTime = gmtime(&currentTime);

	switch (clientChoice)
	{
	case 120:
		sprintf(o_SendBuff, "%02d:%02d:%02d ", utcTime->tm_hour, utcTime->tm_min, utcTime->tm_sec);
		break;
	case 121:
		sprintf(o_SendBuff, "%02d:%02d:%02d ", (utcTime->tm_hour + 3) % 24, utcTime->tm_min, utcTime->tm_sec);
		break;
	case 122:
		sprintf(o_SendBuff, "%02d:%02d:%02d ", (utcTime->tm_hour + 2) % 24, utcTime->tm_min, utcTime->tm_sec);
		break;
	case 123:
		sprintf(o_SendBuff, "%02d:%02d:%02d ", (utcTime->tm_hour - 4) % 24, utcTime->tm_min, utcTime->tm_sec);
		break;
	case 124:
		sprintf(o_SendBuff, "%02d:%02d:%02d ", (utcTime->tm_hour + 2) % 24, utcTime->tm_min, utcTime->tm_sec);
		break;
	default:
		break;
	}
}

// Function to measure time lapse between requests
void MeasureTimeLap(char* o_SendBuff, time_t& o_PrevTime, bool firstRequest)
{
	time_t currentTime = time(NULL);

	if (firstRequest)
	{
		sprintf(o_SendBuff, "%s", "Starting to measure time! ");
	}
	else
	{
		double diffTime = difftime(currentTime, o_PrevTime);

		if (diffTime > 180)
		{
			sprintf(o_SendBuff, "%s", "The measurement was automatically stopped due to timeout. ");
		}
		else
		{
			sprintf(o_SendBuff, "Elapsed time: %.2f seconds ", diffTime);
		}
	}

	o_PrevTime = currentTime;
}