#include "Server.h"

#pragma region Initialization
int Server::init(const char* _address, short _port)
{
	// Create socket
	m_listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_listeningSocket == INVALID_SOCKET)
	{
		printf("DEBUG// Socket function incorrect\n");
		return SETUP_ERROR;
	}
	else
	{
		printf("DEBUG// I used the socket function\n");
	}

	// Bind address/port
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	serverAddr.sin_port = htons(_port);

	int result = bind(m_listeningSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (result == SOCKET_ERROR)
	{
		printf("DEBUG// Bind function incorrect\n");
		return BIND_ERROR;
	}
	else
	{
		printf("DEBUG// I used the bind function\n");
	}

	// Listen
	result = listen(m_listeningSocket, 1);
	if (result == SOCKET_ERROR)
	{
		printf("DEBUG// Listen function incorrect\n");
		return SETUP_ERROR;
	}
	else
	{
		printf("DEBUG// I used the listen function\n");
	}

	printf("Waiting...\n\n");

	// Accept
	m_comSocket = accept(m_listeningSocket, NULL, NULL);
	if (m_comSocket == INVALID_SOCKET)
	{
		printf("DEBUG// Accept function incorrect\n");
		
		int error = WSAGetLastError();
		return (error == WSAESHUTDOWN || error == WSAEDISCON) ? SHUTDOWN : CONNECT_ERROR;
	}
	else
	{
		printf("DEBUG// I used the accept function\n");
	}

	return SUCCESS;
}
Server::Server() : Host(false)
{
	switch (init(nullptr, m_portNumber))
	{
	case SUCCESS:
		break;
	case ADDRESS_ERROR:
		break;
	case BIND_ERROR:
		break;
	case CONNECT_ERROR:
		break;
	case DISCONNECT:
		break;
	case MESSAGE_ERROR:
		break;
	case PARAMETER_ERROR:
		break;
	case SETUP_ERROR:
		break;
	case SHUTDOWN:
		break;
	case STARTUP_ERROR:
		break;
	}
}
#pragma endregion

#pragma region Private Functionality
void Server::stop()
{
	shutdown(m_listeningSocket, SD_BOTH);
	closesocket(m_listeningSocket);

	shutdown(m_comSocket, SD_BOTH);
	closesocket(m_comSocket);
}
#pragma endregion

#pragma region Denitialization
Server::~Server()
{
	stop();
}
#pragma endregion