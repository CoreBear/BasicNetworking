#ifndef HOST_H
#define HOST_H

#define _CRT_SECURE_NO_WARNINGS                 // turns of deprecated warnings
#define _WINSOCK_DEPRECATED_NO_WARNINGS         // turns of deprecated warnings for winsock
#include <winsock2.h>
//#include <ws2tcpip.h>                         // only need if you use inet_pton
#pragma comment(lib,"Ws2_32.lib")
//#include <stdio.h>
#include <fstream>

#include "HelperFunctionality.h"

// Command
#define CCONNECT		"$connect"
#define CDISCONNECT		"$disconnect"
#define CEXIT			"$exit"
#define CGET_LIST		"$get_list"
#define CGET_LOG		"$get_log"
#define CGET_LOG_FINISH	"$get_log_start"
#define CGET_LOG_START	"$get_log_finish"
#define CREGISTER		"$register"

// Response
#define RCONNECTED		"connected"
#define RDISCONNECTED	"disconnected"
#define REXITED			"exited"
#define RGOT_LIST		"got_list"
#define RGOT_LOG		"got_log"
#define RREGISTERED		"registered"

// Return Values
#define SUCCESS  0
#define ADDRESS_ERROR 1
#define BIND_ERROR 2
#define CONNECT_ERROR 3
#define DISCONNECT 4
#define MESSAGE_ERROR 5
#define PARAMETER_ERROR 6
#define SETUP_ERROR 7
#define SHUTDOWN 8
#define STARTUP_ERROR 9

// Server Responses
#define SV_FULL		"Server_Is_Full"
#define SV_SUCCESS	"Server_Connection_Established"

// Status
#define SNOT_CONNECTED 0
#define SCONNECTED 1
#define SREGISTERED 2

class Host
{
#pragma region Variables
private:
	char m_sendBuffer[USHRT_MAX];

protected:
	char m_readBuffer[USHRT_MAX];
	char m_tempBuffer[USHRT_MAX];
	char* m_userName;
	SOCKET m_tcpSocket;
#pragma endregion

#pragma region Initialization
protected:
	Host();
#pragma endregion

#pragma region Update
public:
	virtual void Update() = 0;
#pragma endregion

#pragma region Functionality
public:
	inline bool HostIsConnected() const
	{
		return (m_tcpSocket != INVALID_SOCKET) ? true : false;
	}
	virtual void ReadMess() { return; }
	virtual void SendMess() { return; }

protected:
	virtual int HandleMessageReceive(SOCKET _socket);
	int HandleMessageSend(SOCKET _socket, const char* _message);
	void HandleReturnValue(int _returnValue);

private:
	virtual void CloseSockets() = 0;
	int TCPMessageReceive(SOCKET _socket, char* buf, int len);
	int TCPMessageSend(SOCKET _socket, const char* data, short length);
#pragma endregion

#pragma region Denitialization
public:
	virtual ~Host();
#pragma endregion

};

#endif