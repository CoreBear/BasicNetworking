#ifndef CLIENT_H
#define CLIENT_H

#include "Host.h"

class Client final : public Host
{
#pragma region Variables
private:
	char m_readUDPBuffer[USHRT_MAX];
	sockaddr_in m_servaddr;
	SOCKET m_udpSocket;
#pragma endregion

#pragma region Initialization
public:
	Client();

protected:
	virtual int Initialize(const char* _address, short _port) override;
#pragma endregion

#pragma region Update
public:
	virtual void Update() override;
#pragma endregion

#pragma region Functionality
public:
	virtual void ReadTCP() override;
	virtual void ReadUDP() override;
	virtual void SendTCP() override;

protected:
	virtual int HandleMessageReceive(SOCKET _socket) override;

private:
	virtual void CloseSockets() override;
	void GenerateCommand(const char* _command);
#pragma endregion

#pragma region Denitialization
public:
	virtual ~Client() override;
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