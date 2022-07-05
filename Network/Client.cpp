#include "Client.h"

#pragma region Initialization
Client::Client() : Host(true)
{
	HandleReturnValue(Initialize(m_tempBuffer, m_portNumber));
}
int Client::Initialize(const char* _address, short _port)
{
	m_udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (m_udpSocket == INVALID_SOCKET)
	{
		std::cout << "DEBUG// UDP socket function incorrect" << std::endl;
		return SETUP_ERROR;
	}
	else
	{
		std::cout << "DEBUG// I used the UDP socket function" << std::endl;
	}

	char broadcast = '1';
	int result = setsockopt(m_udpSocket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));

	m_servaddr.sin_family = AF_INET;
	m_servaddr.sin_port = htons(_port);
	m_servaddr.sin_addr.S_un.S_addr = INADDR_ANY;// inet_addr(_address);

	//// Set socket to blocking (0, instead of 1)
	//u_long mode = 0;
	//int result = ioctlsocket(m_udpSocket, FIONBIO, &mode);
	//if (result == SOCKET_ERROR)
	//{
	//	std::cout << "DEBUG// UDP non-block function incorrect" << std::endl;
	//	return BIND_ERROR;
	//}
	//else
	//{
	//	std::cout << "DEBUG// I used the UDP non-block function" << std::endl;
	//}

	result = bind(m_udpSocket, (SOCKADDR*)&m_servaddr, sizeof(m_servaddr));
	if (result == SOCKET_ERROR)
	{
		std::cout << "DEBUG// UCP bind function incorrect" << std::endl;
		return BIND_ERROR;
	}
	else
	{
		std::cout << "DEBUG// I used the UCP bind function" << std::endl;
	}

	// Create socket
	m_tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_tcpSocket == INVALID_SOCKET)
	{
		std::cout << "DEBUG// Socket function incorrect" << std::endl;
		return SETUP_ERROR;
	}
	else
	{
		std::cout << "DEBUG// I used the socket function" << std::endl;
	}

	//// Set socket to non-blocking
	//mode = 1;
	//result = ioctlsocket(m_tcpSocket, FIONBIO, &mode);
	//if (result == SOCKET_ERROR)
	//{
	//	std::cout << "DEBUG// TCP non-block function incorrect" << std::endl;
	//	return BIND_ERROR;
	//}
	//else
	//{
	//	std::cout << "DEBUG// I used the TCP non-block function" << std::endl;
	//}

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

	result = connect(m_tcpSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
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

	// Server will ask for a username
	HandleReturnValue(HandleMessageReceive(m_tcpSocket));
	
	if (strcmp(m_readBuffer, SV_SUCCESS) == 0)
	{
		// Get user name
		HelperFunctionality::GetUserString("Please enter a username: ", m_userName, USHRT_MAX);
		
		GenerateCommand(CREGISTER);

		// Client will return a username
		HandleReturnValue(HandleMessageSend(m_tcpSocket, m_tempBuffer));
		
		return SUCCESS;
	}
	else
	{
		return DISCONNECT;
	}
}
#pragma endregion

#pragma region Update
void Client::Update()
{
}
#pragma endregion

#pragma region Public Functionality
void Client::ReadTCP()
{
	HandleReturnValue(HandleMessageReceive(m_tcpSocket));
}
void Client::ReadUDP()
{
	/*if (m_readUDPBuffer != nullptr)
	{
		memset(m_readUDPBuffer, '\0', USHRT_MAX);
	}*/

	int len = sizeof(struct sockaddr_in);
	int result = recvfrom(m_udpSocket, m_readUDPBuffer, 1024, 0, (SOCKADDR*)&m_servaddr, &len);
	if (result == SOCKET_ERROR)
	{
		std::cout << "Messed up" << std::endl;
	}
	else
	{
		std::cout << m_readUDPBuffer << std::endl;
	}
}
void Client::SendTCP()
{
	HelperFunctionality::GetUserString(nullptr, m_tempBuffer, USHRT_MAX);

	// If a command
	if (m_tempBuffer[0] == '$')
	{
		if (strcmp(m_tempBuffer, CEXIT) == 0)
		{
			GenerateCommand(CEXIT);
			HandleReturnValue(HandleMessageSend(m_tcpSocket, m_tempBuffer));
			CloseSockets();
			return;
		}
		else if (strcmp(m_tempBuffer, CGET_LIST) == 0)
		{
			GenerateCommand(CGET_LIST);
			HandleReturnValue(HandleMessageSend(m_tcpSocket, m_tempBuffer));
			HandleReturnValue(HandleMessageReceive(m_tcpSocket));
			return;
		}
		else if (strcmp(m_tempBuffer, CGET_LOG) == 0)
		{
			GenerateCommand(CGET_LOG);
			HandleReturnValue(HandleMessageSend(m_tcpSocket, m_tempBuffer));
			return;
		}
	}

	HandleReturnValue(HandleMessageSend(m_tcpSocket, m_tempBuffer));
}
#pragma endregion


#pragma region Protected Functionality
int Client::HandleMessageReceive(SOCKET _socket)
{
	int returnValue = Host::HandleMessageReceive(_socket);

	if (returnValue == SUCCESS)
	{
		// Print message to the console
		std::cout << m_readUDPBuffer << std::endl;
	}

	return returnValue;
}
#pragma endregion

#pragma region Private Functionality
void Client::CloseSockets()
{
	shutdown(m_tcpSocket, SD_BOTH);
	closesocket(m_tcpSocket);

	m_tcpSocket = INVALID_SOCKET;
}
void Client::GenerateCommand(const char* _command)
{
	memset(m_tempBuffer, '\0', USHRT_MAX);
	strcpy(m_tempBuffer, _command);
	strcat(m_tempBuffer, " ");
	strcat(m_tempBuffer, m_userName);
}
#pragma endregion

#pragma region Denitialization
Client::~Client()
{
	CloseSockets();
}
#pragma endregion