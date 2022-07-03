#ifndef CLIENT_H
#define CLIENT_H

#include "Host.h"

class Client final : public Host
{
#pragma region Initialization
public:
	Client();

protected:
	int Initialize(const char* _address, short _port);
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
	virtual ~Client();
#pragma endregion

	//1. Prompt the user for the server's IP address and port number (this step will be replaced by the information received on the UDP connection in phase 2)
	//2. Create a TCP socket
	//3. Connect to the server using the information provided in step 1
	//4. Prompt for a username
	//5. Call the command $register.The username must be automatically amended to the command before being sent.E.g.$register < username>
	//6. Receive the server response on the socket.If SV_FULL, close the socketand exit program
	//7. If SV_SUCCESS, the client can execute commandsand send chat messages.
};

#endif