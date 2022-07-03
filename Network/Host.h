#ifndef HOST_H
#define HOST_H

#define _CRT_SECURE_NO_WARNINGS                 // turns of deprecated warnings
#define _WINSOCK_DEPRECATED_NO_WARNINGS         // turns of deprecated warnings for winsock
#include <winsock2.h>
//#include <ws2tcpip.h>                         // only need if you use inet_pton
#pragma comment(lib,"Ws2_32.lib")
//#include <stdio.h>

#include "HelperFunctionality.h"

class Host
{
#pragma region Variables
protected:
	char m_buffer[USHRT_MAX];
	SOCKET m_comSocket;
	ushort m_portNumber;
#pragma endregion

#pragma region Initialization
protected:
	Host(bool _getIPFromUser);
	virtual int init(const char* _address, short _port) = 0;
#pragma endregion

#pragma region Public Functionality
public:
	int readMessage();
	int sendMessage();
	virtual void stop() = 0;
#pragma endregion

#pragma region Private Functionality
private:
	int tcp_recv_whole(SOCKET s, char* buf, int len);
	int tcp_send_whole(SOCKET skSocket, const char* data, short length);
#pragma endregion

#pragma region Denitialization
public:
	virtual ~Host();
#pragma endregion

};

#endif