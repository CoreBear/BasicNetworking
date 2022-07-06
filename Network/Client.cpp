#include "Client.h"

#pragma region Initialization
Client::Client() : m_islogging(false)
{
	HandleReturnValue(Initialize());
}
int Client::Initialize()
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
	int result = setsockopt(m_udpSocket, SOL_SOCKET, SO_REUSEADDR, &broadcast, sizeof(broadcast));
	
	// Connect
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	serverAddr.sin_port = htons(1);

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

	result = bind(m_udpSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (result == SOCKET_ERROR)
	{
		int erno = WSAGetLastError();
		std::cout << "DEBUG// UDP bind function incorrect" << std::endl;
		return BIND_ERROR;
	}
	else
	{
		std::cout << "DEBUG// I used the UDP bind function" << std::endl;
	}

	int len = sizeof(struct sockaddr_in);
	result = recvfrom(m_udpSocket, m_readBuffer, 1024, 0, (SOCKADDR*)&serverAddr, &len);
	if (result == SOCKET_ERROR)
	{
		// HACK: Make a better error
		std::cout << "Messed up" << std::endl;
	}
	else
	{
		std::cout << m_readBuffer << std::endl;
	}

	shutdown(m_udpSocket, SD_BOTH);
	closesocket(m_udpSocket);

	m_udpSocket = INVALID_SOCKET;

	// Get IP address from user
	HelperFunctionality::GetValidIPAddress(m_tempBuffer, USHRT_MAX);

	// Get port number from user
	constexpr int LOWEST_PORT_NUMBER_AVAILABLE = 1;
	ushort port = static_cast<short>(HelperFunctionality::GetUserInt("Please enter a port number", USHRT_MAX, LOWEST_PORT_NUMBER_AVAILABLE));


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
	if (inet_addr(m_tempBuffer) == INADDR_NONE)
	{
		return ADDRESS_ERROR;
	}

	//// Connect
	sockaddr_in serverAddr2;
	serverAddr2.sin_family = AF_INET;
	serverAddr2.sin_addr.S_un.S_addr = inet_addr(m_tempBuffer);
	serverAddr2.sin_port = htons(port);

	result = connect(m_tcpSocket, (SOCKADDR*)&serverAddr2, sizeof(serverAddr2));
	if (result == SOCKET_ERROR)
	{
		int erno = WSAGetLastError();
		std::cout << "DEBUG// Connect function incorrect" << std::endl;
		
		int error = WSAGetLastError();
		return (error == WSAESHUTDOWN || error == WSAEDISCON) ? SHUTDOWN : CONNECT_ERROR;
	}
	else
	{
		std::cout << "DEBUG// I used the Connect function" << std::endl;
	}

	// Server returns connection response
	HandleReturnValue(HandleMessageReceive(m_tcpSocket));
	
	if (strcmp(m_readBuffer, SV_SUCCESS) == 0)
	{
		m_connectionStatus = SCONNECTED;
		std::cout << "Connected to the server!" << std::endl;
		return SUCCESS;
	}
	else
	{
		m_connectionStatus = SNOT_CONNECTED;
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
void Client::ReadMess()
{
	// If client is connected or registered
	if (m_connectionStatus > SNOT_CONNECTED)
	{
		// Read TCP
		HandleReturnValue(HandleMessageReceive(m_tcpSocket));

		if (strcmp(m_readBuffer, RREGISTERED) == 0)
		{
			//std::cout << "Registered to the server!" << std::endl;
			m_connectionStatus = SREGISTERED;
		}

		// If client is just connected, read UDP
		if (m_connectionStatus == SCONNECTED)
		{
		}
	}
}
void Client::SendMess()
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
			if (m_connectionStatus == SREGISTERED)
			{
				GenerateCommand(CGET_LIST);
			}
			else
			{
				std::cout << "Must be registered to the server before this command can be performed!" << std::endl;

				return;
			}
		}
		else if (strcmp(m_tempBuffer, CGET_LOG) == 0)
		{
			if (m_connectionStatus == SREGISTERED)
			{
				GenerateCommand(CGET_LOG);
			}
			else
			{
				std::cout << "Must be registered to the server before this command can be performed!" << std::endl;

				return;
			}
		}
		else if (strcmp(m_tempBuffer, CREGISTER) == 0)
		{
			if (m_connectionStatus != SREGISTERED)
			{
				// Get user name
				HelperFunctionality::GetUserString("Please enter a username: ", m_userName, USHRT_MAX);

				GenerateCommand(CREGISTER);
			}
			else
			{
				std::cout << "You're already registered to the server!" << std::endl;

				return;
			}
		}

		HandleReturnValue(HandleMessageSend(m_tcpSocket, m_tempBuffer));
	}
	else
	{
		if (m_connectionStatus == SREGISTERED)
		{
			HandleReturnValue(HandleMessageSend(m_tcpSocket, m_tempBuffer));
		}
		else
		{
			std::cout << "Must be registered to the server before messages can be sent, with the exeption of " <<  CEXIT << " & " << CREGISTER << "!" << std::endl;
		}
	}
}
#pragma endregion


#pragma region Protected Functionality
int Client::HandleMessageReceive(SOCKET _socket)
{
	int returnValue = Host::HandleMessageReceive(_socket);

	if (returnValue == SUCCESS)
	{
		if (strcmp(m_readBuffer, CGET_LOG_START) == 0)
		{
			m_islogging = true;
			memset(m_tempBuffer, '\0', USHRT_MAX);
			strcpy(m_tempBuffer, m_userName);
			strcat(m_tempBuffer, ".txt");
			m_outStream.open(m_tempBuffer, std::ios::out);
		}
		else if (strcmp(m_readBuffer, CGET_LOG_FINISH) == 0)
		{
			m_islogging = false;
			m_outStream.close();
		}

		if (m_islogging)
		{
			if (m_outStream.is_open())
			{
				m_outStream << m_readBuffer << '\n';
			}
		}

		std::cout << m_readBuffer << std::endl;
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
	m_connectionStatus = SNOT_CONNECTED;
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