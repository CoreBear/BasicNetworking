#include "Client.h"

#pragma region Initialization
Client::Client() : Host(true)
{
	HandleReturnValue(Initialize(m_buffer, m_portNumber));
}
int Client::Initialize(const char* _address, short _port)
{
	// Create socket
	m_comSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_comSocket == INVALID_SOCKET)
	{
		std::cout << "DEBUG// Socket function incorrect" << std::endl;
		return SETUP_ERROR;
	}
	else
	{
		std::cout << "DEBUG// I used the socket function" << std::endl;
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
		std::cout << "DEBUG// Connect function incorrect" << std::endl;
		
		int error = WSAGetLastError();
		return (error == WSAESHUTDOWN || error == WSAEDISCON) ? SHUTDOWN : CONNECT_ERROR;
	}
	else
	{
		std::cout << "DEBUG// I used the Connect function" << std::endl;
	}

	return SUCCESS;
}
#pragma endregion

#pragma region Update
void Client::Update()
{
	while (true)
	{
		HandleReturnValue(HandleMessageSend());
	}
}
#pragma endregion

#pragma region Private Functionality
void Client::CloseSockets()
{
	shutdown(m_comSocket, SD_BOTH);
	closesocket(m_comSocket);
}
#pragma endregion

#pragma region Denitialization
Client::~Client()
{
	CloseSockets();
}
#pragma endregion