#include "Server.h"

#pragma region Initialization
int Server::Initialize(short _port)
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
	//// Set socket to non-blocking
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

	// Bind address/port
	m_serverAddr.sin_family = AF_INET;
	m_serverAddr.sin_addr.S_un.S_addr = INADDR_BROADCAST;// inet_addr("127.0.0.1");// 255.255");
	m_serverAddr.sin_port = htons(1);

	//result = bind(m_udpSocket, (SOCKADDR*)&m_serverAddr, sizeof(m_serverAddr));
	//if (result == SOCKET_ERROR)
	//{
	//	std::cout << "DEBUG// UDP bind function incorrect" << std::endl;
	//	return BIND_ERROR;
	//}
	//else
	//{
	//	std::cout << "DEBUG// I used the UDP bind function" << std::endl;
	//}

	std::sprintf(m_portString, "%d", _port);

	memset(m_tempBuffer, '\0', USHRT_MAX);
	strcpy(m_tempBuffer, "IP_Address: ");
	strcat(m_tempBuffer, m_addressNumber);
	strcat(m_tempBuffer, " | ");
	strcat(m_tempBuffer, "Port_Number: ");
	strcat(m_tempBuffer, m_portString);

	sendto(m_udpSocket, m_tempBuffer, static_cast<int>(strlen(m_tempBuffer) + 1), 0, (SOCKADDR*)&m_serverAddr, sizeof(m_serverAddr));

	// Create tcp socket for listening
	m_tcpSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_tcpSocket == INVALID_SOCKET)
	{
		std::cout << "DEBUG// TCP socket function incorrect" << std::endl;
		return SETUP_ERROR;
	}
	else
	{
		std::cout << "DEBUG// I used the TCP socket function" << std::endl;
	}

	// Bind address/port
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	serverAddr.sin_port = htons(_port);

	result = bind(m_tcpSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (result == SOCKET_ERROR)
	{
		std::cout << "DEBUG// TCP bind function incorrect" << std::endl;
		return BIND_ERROR;
	}
	else
	{
		std::cout << "DEBUG// I used the TCP bind function" << std::endl;
	}

	// Listen
	result = listen(m_tcpSocket, m_maxNumberOfUsers);
	if (result == SOCKET_ERROR)
	{
		std::cout << "DEBUG// Listen function incorrect" << std::endl;
		return SETUP_ERROR;
	}
	else
	{
		std::cout << "DEBUG// I used the listen function" << std::endl;
	}

	std::cout << "Waiting..." << std::endl;

	std::ofstream outStream("Log.txt", std::ios::out | std::ios::trunc);

	if (outStream.is_open())
	{
		//outStream << _logPrintString;

		outStream.close();
	}

	return SUCCESS;
}
Server::Server()
{
	// Examples
	// IP (Loopback) - "127.0.0.1"
	// Port - 31337
	
	// Get IP address from user
	HelperFunctionality::GetValidIPAddress(m_addressNumber, USHRT_MAX);

	// Get port number from user
	constexpr int LOWEST_PORT_NUMBER_AVAILABLE = 1;
	m_portNumber = static_cast<short>(HelperFunctionality::GetUserInt("Please enter a port number", USHRT_MAX, LOWEST_PORT_NUMBER_AVAILABLE));

	strcpy(m_userName, "HostDaddy");

	constexpr int MAX_NUMBER_OF_USERS = 4, MIN_NUMBER_OF_USERS = 1;
	m_maxNumberOfUsers = HelperFunctionality::GetUserInt("Enter max number of users: ", MAX_NUMBER_OF_USERS, MIN_NUMBER_OF_USERS);
	// Add an additional, to account for the server
	m_maxNumberOfUsers++;

	HandleReturnValue(Initialize(m_portNumber));

	m_timeVal.tv_sec = 1;

	m_connectedAndOrRegisteredUsers.reserve(m_maxNumberOfUsers);

	User* server = new User();
	server->m_socket = m_tcpSocket;
	server->m_userName = m_userName;

	m_connectedAndOrRegisteredUsers.push_back(server);
}
#pragma endregion

#pragma region Update
void Server::Update()
{
	// Reset/Init the descriptor set (the sockets used in select)
	for (size_t i = 0, j; i < static_cast<size_t>(DescriptorType::NumberOfTypes); i++)
	{
		// Reset all of them
		FD_ZERO(&m_readyDescriptors[i]);

		// For each possible connection
		for (j = 0; j < m_connectedAndOrRegisteredUsers.size(); j++)
		{
			// Add it to the descriptor set
			FD_SET(m_connectedAndOrRegisteredUsers[j]->m_socket, &m_readyDescriptors[i]);
		}
	}

	int result = select(static_cast<size_t>(DescriptorType::NumberOfTypes), &m_readyDescriptors[static_cast<size_t>(DescriptorType::Read)], NULL, NULL, &m_timeVal);

	memset(m_tempBuffer, '\0', USHRT_MAX);
	strcpy(m_tempBuffer, "IP_Address: ");
	strcat(m_tempBuffer, m_addressNumber);
	strcat(m_tempBuffer, " | ");
	strcat(m_tempBuffer, "Port_Number: ");
	strcat(m_tempBuffer, m_portString);

	sendto(m_udpSocket, m_tempBuffer, static_cast<int>(strlen(m_tempBuffer) + 1), 0, (SOCKADDR*)&m_serverAddr, sizeof(m_serverAddr));

	// Error occurred during select
	if (result < 0)
	{
	}

	// None of the sockets are ready
	else if (result == 0)
	{
	}

	// At least one socket is ready
	else
	{
		// Check if the host with event is the server
		if (FD_ISSET(m_tcpSocket, &m_readyDescriptors[static_cast<int>(DescriptorType::Read)]))
		{
			// Accept new connection
			m_clientReadySocket = accept(m_tcpSocket, NULL, NULL);
			if (m_clientReadySocket == INVALID_SOCKET)
			{
				std::cout << "DEBUG// Accept function incorrect" << std::endl;
			}

			// If server is not full
			if (m_connectedAndOrRegisteredUsers.size() != m_connectedAndOrRegisteredUsers.capacity())
			{
				User* server = new User();
				server->m_userName = nullptr;
				server->m_socket = m_clientReadySocket;

				// Add the new connection
				m_connectedAndOrRegisteredUsers.push_back(server);

				// Send message to connecting client
				HandleReturnValue(HandleMessageSend(m_clientReadySocket, SV_SUCCESS));
				
				memset(m_logBuffer, '\0', USHRT_MAX);
				strcpy(m_logBuffer, "New client ");
				strcat(m_logBuffer, RCONNECTED);
				LogAndPrintToConsole(m_logBuffer);
			}

			// If server is full
			else
			{
				HandleReturnValue(HandleMessageSend(m_clientReadySocket, SV_FULL));
			}

			// If there were no more ready descriptors, the update is complete
			if (--result > 0)
			{
				return;
			}
		}

		// NOTE: Check if the host with event is a non-server
		// For each possible connection
		for (size_t i = 1; i < m_connectedAndOrRegisteredUsers.size(); i++)
		{
			// If connection is valid and it is ready
			if (FD_ISSET(m_connectedAndOrRegisteredUsers[i]->m_socket, &m_readyDescriptors[static_cast<size_t>(DescriptorType::Read)]))
			{
				// Handle its message and if returns anything other than success, disconnect the socket
				if (HandleMessageReceive(m_connectedAndOrRegisteredUsers[i]->m_socket) != SUCCESS)
				{
					DisconnectUser(i);
				}

				// If successful message was received
				else
				{
					// If not a command
					if (m_readBuffer[0] != '$')
					{
						memset(m_logBuffer, '\0', USHRT_MAX);
						strcpy(m_logBuffer, m_connectedAndOrRegisteredUsers[i]->m_userName);
						strcat(m_logBuffer, ": ");
						strcat(m_logBuffer, m_readBuffer);
						LogAndPrintToConsole(m_logBuffer);

						memset(m_tempBuffer, '\0', USHRT_MAX);
						strcpy(m_tempBuffer, m_connectedAndOrRegisteredUsers[i]->m_userName);
						strcat(m_tempBuffer, ": ");
						strcat(m_tempBuffer, m_readBuffer);

						for (size_t j = 1; j < m_connectedAndOrRegisteredUsers.size(); j++)
						{
							if (i != j)
							{
								HandleReturnValue(HandleMessageSend(m_connectedAndOrRegisteredUsers[j]->m_socket, m_tempBuffer));
							}
						}
					}

					// If attempted command
					else
					{
						HandleCommand(i);
					}
				}
			}
		}
	}
}
#pragma endregion

#pragma region Private Functionality
void Server::CloseSockets()
{
	shutdown(m_clientReadySocket, SD_BOTH);
	closesocket(m_clientReadySocket);

	shutdown(m_tcpSocket, SD_BOTH);
	closesocket(m_tcpSocket);

	for (size_t i = 0; i < m_connectedAndOrRegisteredUsers.size(); i++)
	{
		shutdown(m_connectedAndOrRegisteredUsers[i]->m_socket, SD_BOTH);
		closesocket(m_connectedAndOrRegisteredUsers[i]->m_socket);
	}
}
void Server::DisconnectUser(size_t _userBeingDisconnectedIndex)
{
	if (m_connectedAndOrRegisteredUsers[_userBeingDisconnectedIndex]->m_userName != nullptr)
	{
		memset(m_logBuffer, '\0', USHRT_MAX);
		strcpy(m_logBuffer, RDISCONNECTED);
		strcat(m_logBuffer, ": ");
		strcat(m_logBuffer, m_connectedAndOrRegisteredUsers[_userBeingDisconnectedIndex]->m_userName);
		LogAndPrintToConsole(m_logBuffer);

		memset(m_tempBuffer, '\0', USHRT_MAX);
		strcpy(m_tempBuffer, RDISCONNECTED);
		strcat(m_tempBuffer, " ");
		strcat(m_tempBuffer, m_connectedAndOrRegisteredUsers[_userBeingDisconnectedIndex]->m_userName);

		for (size_t i = 1; i < m_connectedAndOrRegisteredUsers.size(); i++)
		{
			if (i != _userBeingDisconnectedIndex)
			{
				HandleReturnValue(HandleMessageSend(m_connectedAndOrRegisteredUsers[i]->m_socket, m_tempBuffer));
			}
		}
	}
	else
	{
		memset(m_logBuffer, '\0', USHRT_MAX);
		strcpy(m_logBuffer, RDISCONNECTED);
		strcat(m_logBuffer, " no name user!");
		LogAndPrintToConsole(m_logBuffer);
	}

	shutdown(m_connectedAndOrRegisteredUsers[_userBeingDisconnectedIndex]->m_socket, SD_BOTH);
	closesocket(m_connectedAndOrRegisteredUsers[_userBeingDisconnectedIndex]->m_socket);

	// Remove user
	delete m_connectedAndOrRegisteredUsers[_userBeingDisconnectedIndex];
	m_connectedAndOrRegisteredUsers[_userBeingDisconnectedIndex] = nullptr;
	m_connectedAndOrRegisteredUsers.erase(m_connectedAndOrRegisteredUsers.begin() + _userBeingDisconnectedIndex);
}
void Server::HandleCommand(size_t _userBeingReadIndex)
{
	size_t userNameLength = 0, userNameStartIndex = 0;
	const char* walker = m_readBuffer;

	// Walk to the start of the user's name and count how many steps
	{
		while (*walker != ' ')
		{
			userNameStartIndex++;
			walker++;
		}

		userNameStartIndex++;
		walker++;
	}

	// Walk to the end of the user's name and count how many steps
	{
		while (*walker != '\0')
		{
			userNameLength++;
			walker++;
		}

		userNameLength++;
	}

	// Point to the beginning of the user's name
	walker = &m_readBuffer[userNameStartIndex];

	// Allocate memory for user's name and copy over user's name
	char* userName = new char[userNameLength];
	for (size_t i = 0; i < userNameLength; i++, walker++)
	{
		userName[i] = *walker;
	}

	userName[userNameLength - OFF_BY_ONE] = '\0';

	// Cut off user's name from the buffer (and the accompanying space)
	m_readBuffer[--userNameStartIndex] = '\0';

	if (strcmp(m_readBuffer, CEXIT) == 0)
	{
		DisconnectUser(_userBeingReadIndex);
	}
	else if (strcmp(m_readBuffer, CGET_LIST) == 0)
	{
		memset(m_tempBuffer, '\0', USHRT_MAX);
		strcpy(m_tempBuffer, "Connected users:");
		for (size_t i = 1; i < m_connectedAndOrRegisteredUsers.size(); i++)
		{
			strcat(m_tempBuffer, " ");

			if (m_connectedAndOrRegisteredUsers[i]->m_userName != nullptr)
			{
				strcat(m_tempBuffer, m_connectedAndOrRegisteredUsers[i]->m_userName);
			}
			else
			{
				strcat(m_tempBuffer, "NA");
			}
		}

		HandleReturnValue(HandleMessageSend(m_connectedAndOrRegisteredUsers[_userBeingReadIndex]->m_socket, m_tempBuffer));

		memset(m_logBuffer, '\0', USHRT_MAX);
		strcpy(m_logBuffer, RGOT_LIST);
		strcat(m_logBuffer, " ");
		strcat(m_logBuffer, userName);
		LogAndPrintToConsole(m_logBuffer);

		memset(m_tempBuffer, '\0', USHRT_MAX);
		strcpy(m_tempBuffer, RGOT_LIST);
		strcat(m_tempBuffer, " ");
		strcat(m_tempBuffer, userName);

		for (size_t i = 1; i < m_connectedAndOrRegisteredUsers.size(); i++)
		{
			if (i != _userBeingReadIndex)
			{
				HandleReturnValue(HandleMessageSend(m_connectedAndOrRegisteredUsers[i]->m_socket, m_tempBuffer));
			}
		}
	}
	else if (strcmp(m_readBuffer, CGET_LOG) == 0)
	{
		std::ifstream inStream("Log.txt", std::ios::in);

		HandleReturnValue(HandleMessageSend(m_connectedAndOrRegisteredUsers[_userBeingReadIndex]->m_socket, CGET_LOG_START));

		if (inStream.is_open())
		{
			while (inStream.getline(m_tempBuffer, USHRT_MAX, '\n'))
			{
				HandleReturnValue(HandleMessageSend(m_connectedAndOrRegisteredUsers[_userBeingReadIndex]->m_socket, m_tempBuffer));
			}

			inStream.close();
		}

		HandleReturnValue(HandleMessageSend(m_connectedAndOrRegisteredUsers[_userBeingReadIndex]->m_socket, CGET_LOG_FINISH));

		memset(m_logBuffer, '\0', USHRT_MAX);
		strcpy(m_logBuffer, RGOT_LOG);
		strcat(m_logBuffer, " ");
		strcat(m_logBuffer, userName);
		LogAndPrintToConsole(m_logBuffer);

		memset(m_tempBuffer, '\0', USHRT_MAX);
		strcpy(m_tempBuffer, RGOT_LOG);
		strcat(m_tempBuffer, " ");
		strcat(m_tempBuffer, userName);

		for (size_t i = 1; i < m_connectedAndOrRegisteredUsers.size(); i++)
		{
			if (i != _userBeingReadIndex)
			{
				HandleReturnValue(HandleMessageSend(m_connectedAndOrRegisteredUsers[i]->m_socket, m_tempBuffer));
			}
		}
	}
	else if (strcmp(m_readBuffer, CREGISTER) == 0)
	{
		m_connectedAndOrRegisteredUsers[_userBeingReadIndex]->m_userName = userName;
		
		memset(m_tempBuffer, '\0', USHRT_MAX);
		strcpy(m_tempBuffer, RREGISTERED);

		HandleReturnValue(HandleMessageSend(m_connectedAndOrRegisteredUsers[_userBeingReadIndex]->m_socket, m_tempBuffer));

		memset(m_logBuffer, '\0', USHRT_MAX);
		strcpy(m_logBuffer, RREGISTERED);
		strcat(m_logBuffer, " ");
		strcat(m_logBuffer, userName);
		LogAndPrintToConsole(m_logBuffer);

		//memset(m_tempBuffer, '\0', USHRT_MAX);
		//strcpy(m_tempBuffer, RREGISTERED);
		strcat(m_tempBuffer, " ");
		strcat(m_tempBuffer, userName);
		
		for (size_t i = 1; i < m_connectedAndOrRegisteredUsers.size(); i++)
		{
			if (i != _userBeingReadIndex)
			{
				HandleReturnValue(HandleMessageSend(m_connectedAndOrRegisteredUsers[i]->m_socket, m_tempBuffer));
			}
		}
	}
	else
	{
		memset(m_logBuffer, '\0', USHRT_MAX);
		strcpy(m_logBuffer, m_connectedAndOrRegisteredUsers[_userBeingReadIndex]->m_userName);
		strcat(m_logBuffer, ": ");
		strcat(m_logBuffer, m_readBuffer);
		LogAndPrintToConsole(m_logBuffer);

		memset(m_tempBuffer, '\0', USHRT_MAX);
		strcpy(m_tempBuffer, m_connectedAndOrRegisteredUsers[_userBeingReadIndex]->m_userName);
		strcat(m_tempBuffer, ": ");
		strcat(m_tempBuffer, m_readBuffer);

		for (size_t i = 1; i < m_connectedAndOrRegisteredUsers.size(); i++)
		{
			HandleReturnValue(HandleMessageSend(m_connectedAndOrRegisteredUsers[i]->m_socket, m_tempBuffer));
		}
	}
}
void Server::LogAndPrintToConsole(const char* _logPrintString)
{
	std::ofstream outStream("Log.txt", std::ios::out | std::ios::app);

	if (outStream.is_open())
	{
		outStream << _logPrintString << '\n';

		outStream.close();
	}

	std::cout << _logPrintString << std::endl;
}
#pragma endregion

#pragma region Denitialization
Server::~Server()
{
	// For each possible user
	for (size_t i = 0; i < m_connectedAndOrRegisteredUsers.size(); i++)
	{
		delete m_connectedAndOrRegisteredUsers[i];
		m_connectedAndOrRegisteredUsers[i] = nullptr;;
	}

	m_connectedAndOrRegisteredUsers.clear();

	CloseSockets();
}
#pragma endregion