#include "Server.h"

#pragma region Initialization
int Server::Initialize(const char* _address, short _port)
{
	// Create socket
	m_listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_listeningSocket == INVALID_SOCKET)
	{
		std::cout << "DEBUG// Socket function incorrect" << std::endl;
		return SETUP_ERROR;
	}
	else
	{
		std::cout << "DEBUG// I used the socket function" << std::endl;
	}

	// Set socket to non-blocking
	/*u_long mode = 0;
	int result = ioctlsocket(m_listeningSocket, FIONBIO, &mode);
	if (result == SOCKET_ERROR)
	{
		std::cout << "DEBUG// Non-block function incorrect" << std::endl;
		return BIND_ERROR;
	}
	else
	{
		std::cout << "DEBUG// I used the non-block function" << std::endl;
	}*/

	// Bind address/port
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	serverAddr.sin_port = htons(_port);

	int result = bind(m_listeningSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
	if (result == SOCKET_ERROR)
	{
		std::cout << "DEBUG// Bind function incorrect" << std::endl;
		return BIND_ERROR;
	}
	else
	{
		std::cout << "DEBUG// I used the bind function" << std::endl;
	}

	// Listen
	result = listen(m_listeningSocket, MAX_CONNECTIONS);
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

	return SUCCESS;
}
Server::Server() : Host(false)
{
	HandleReturnValue(Initialize(nullptr, m_portNumber));

	// Invalidate all sockets
	for (size_t i = 0; i < MAX_CONNECTIONS; i++)
	{
		m_allSockets[i] = INVALID_SOCKET;
	}

	// Add the server's listening socket to the container
	m_allSockets[0] = m_listeningSocket;
}
#pragma endregion

#pragma region Update
void Server::Update()
{
	// Initialize the descriptor set (the sockets used in select)
	for (size_t i = 0, j; i < static_cast<size_t>(DescriptorType::NumberOfTypes); i++)
	{
		// Reset all of them
		FD_ZERO(&m_readyDescriptors[i]);

		// For each possible connection
		for (j = 0; j < MAX_CONNECTIONS; j++)
		{
			// If this element is a valid socket
			if (m_allSockets[j] != INVALID_SOCKET)
			{
				// Add it to the descriptor set
				FD_SET(m_allSockets[j], &m_readyDescriptors[i]);
			}
		}
	}

	// HACK: For now, wait forever (last NULL)
	int result = select(static_cast<size_t>(DescriptorType::NumberOfTypes), &m_readyDescriptors[static_cast<size_t>(DescriptorType::Read)], &m_readyDescriptors[static_cast<size_t>(DescriptorType::Write)], NULL, NULL);// &m_readyDescriptors[static_cast<size_t>(DescriptorType::Write)], & m_readyDescriptors[static_cast<size_t>(DescriptorType::Exception)], NULL);

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
		if (FD_ISSET(m_listeningSocket, &m_readyDescriptors[static_cast<int>(DescriptorType::Read)]))
		{
			// Accept new connection
			m_comSocket = accept(m_listeningSocket, NULL, NULL);
			if (m_comSocket == INVALID_SOCKET)
			{
				std::cout << "DEBUG// Accept function incorrect" << std::endl;
			}

			// For each possible connection
			for (size_t i = 0; i < MAX_CONNECTIONS; i++)
			{
				// If this element is an invalid socket
				if (m_allSockets[i] == INVALID_SOCKET)
				{
					// Add the new connection and stop looking for a place to put it
					m_allSockets[i] = m_comSocket;
					break;
				}
			}

			// If there were no more ready descriptors, the update is complete
			if (--result > 0)
			{
				return;
			}
		}

		// NOTE: Check if the host with event is a non-server
		// For each possible connection
		for (size_t i = 1; i < MAX_CONNECTIONS; i++)
		{
			// If connection is valid and it is ready
			if (m_allSockets[i] != INVALID_SOCKET && FD_ISSET(m_allSockets[i], &m_readyDescriptors[static_cast<size_t>(DescriptorType::Read)]))
			{
				// Handle its message and if returns anything other than success, disconnect the socket
				if (HandleMessageReceive(m_allSockets[i]) != SUCCESS)
				{
					m_allSockets[i] = INVALID_SOCKET;
				}
			}
		}
	}
}
#pragma endregion

#pragma region Private Functionality
void Server::CloseSockets()
{
	shutdown(m_comSocket, SD_BOTH);
	closesocket(m_comSocket);

	shutdown(m_listeningSocket, SD_BOTH);
	closesocket(m_listeningSocket);

	for (size_t i = 0; i < MAX_CONNECTIONS; i++)
	{
		shutdown(m_allSockets[i], SD_BOTH);
		closesocket(m_allSockets[i]);
	}
}
#pragma endregion

#pragma region Denitialization
Server::~Server()
{
	CloseSockets();
}
#pragma endregion