#ifndef SERVER_H
#define SERVER_H

#include "Host.h"

class Server final : public Host
{
#pragma region Variables
private:
	enum class DescriptorType { Exception, Read, Write, NumberOfTypes };
	fd_set m_readyDescriptors[static_cast<size_t>(DescriptorType::NumberOfTypes)];
	static const int MAX_CONNECTIONS = 5;
	SOCKET m_allSockets[MAX_CONNECTIONS];
	SOCKET m_listeningSocket;
#pragma endregion

#pragma region Initialization
protected:
	int Initialize(const char* _address, short _port);

public:
	Server();
#pragma endregion

#pragma region Update
public:
	void Update();
#pragma endregion

#pragma region Functionality
private:
	void CloseSockets();
#pragma endregion

#pragma region Denitialization
public:
	virtual ~Server();
#pragma endregion

	//1. Create the TCP listening socket
	//2. Bind the socket
	//3. Set the server in listening mode
	//4. Create the file descriptor sets master set and ready set to multiplex the connected and connecting clients.
	//5. Call select and return after timeout 1 second(NB: if timeout is null, select will note return unless
	//6. Check if the listening socket is SET in the ready set using the macro FD_ISSET().If so, accept the connection, add the client to the list of connected clients1, and add the new socket to the master set.
	//7. Run through the ready set and recv data from the socket.Make sure to check if the current socket is the listening socket before you call recv().
	//8. Parse the received message, and perform the correspondent action(s) : o Handle the $register command : check the chat capacity, send an SV_FULL message if full, or SV_SUCCESS otherwise.In case of SV_FULL, close the socket.
};

#endif