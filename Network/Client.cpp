#include "Client.h"

#pragma region Initialization
Client::Client() : Host(true)
{
	switch (init(m_buffer, m_portNumber))
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
int Client::init(const char* _address, short _port)
{
	// Create socket
	m_comSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_comSocket == INVALID_SOCKET)
	{
		printf("DEBUG// Socket function incorrect\n");
		return SETUP_ERROR;
	}
	else
	{
		printf("DEBUG// I used the socket function\n");
	}

	//If address is not in dotted - quadrant format, returns ADDRESS_ERROR.
	if (inet_addr(_address) == INADDR_NONE)
	{
		return ADDRESS_ERROR;
	}

	// Connect
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.S_un.S_addr = inet_addr(_address);
	serverAddr.sin_port = htons(_port);

	int result = connect(m_comSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (result == SOCKET_ERROR)
	{
		printf("DEBUG// Connect function incorrect\n");
		
		int error = WSAGetLastError();
		return (error == WSAESHUTDOWN || error == WSAEDISCON) ? SHUTDOWN : CONNECT_ERROR;
	}
	else
	{
		printf("DEBUG// I used the Connect function\n");
	}

	return SUCCESS;
}
#pragma endregion

#pragma region Private Functionality
void Client::stop()
{
	shutdown(m_comSocket, SD_BOTH);
	closesocket(m_comSocket);
}
#pragma endregion

#pragma region Denitialization
Client::~Client()
{
	stop();
}
#pragma endregion