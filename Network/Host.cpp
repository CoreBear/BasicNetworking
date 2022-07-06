#include "Host.h"

#pragma region Initialization
Host::Host() : m_tcpSocket(0)
{
	// Initiates use of WS2_32.DLL by a processand must be called before any network functions!
	WSADATA wsadata;
	int result = WSAStartup(WINSOCK_VERSION, &wsadata);

	m_userName = new char[USHRT_MAX];
}
#pragma endregion

#pragma region Protected Functionality
int Host::HandleMessageReceive(SOCKET _socket)
{
	// Variable for how big the message (partial or full) is/will be
	char size = 0;

	// Read just the header to see how big the message will be
	int result = TCPMessageReceive(_socket, (char*)&size, 1);
	if ((result == SOCKET_ERROR) || (result == 0))
	{
		printf("DEBUG// recv is incorrect\n");

		int error = WSAGetLastError();
		return (error == WSAESHUTDOWN || error == WSAEDISCON) ? SHUTDOWN : CONNECT_ERROR;
	}

	// Read the message
	result = TCPMessageReceive(_socket, m_readBuffer, size);
	if ((result == SOCKET_ERROR) || (result == 0))
	{
		printf("DEBUG// recv is incorrect\n");

		int error = WSAGetLastError();
		return (error == WSAESHUTDOWN || error == WSAEDISCON) ? SHUTDOWN : CONNECT_ERROR;
	}

	return SUCCESS;
}
int Host::HandleMessageSend(SOCKET _socket, const char* _message)
{
	strcpy(m_sendBuffer, _message);

	byte size = static_cast<byte>(strlen(m_sendBuffer) + OFF_BY_ONE);
	
	// Send the header so the receiver knows how big the message will be
	int result = TCPMessageSend(_socket, (char*)&size, 1);
	if ((result == SOCKET_ERROR) || (result == 0))
	{
		printf("DEBUG// send is incorrect\n");

		int error = WSAGetLastError();
		return (error == WSAESHUTDOWN || error == WSAEDISCON) ? SHUTDOWN : CONNECT_ERROR;
	}

	// Send the message
	result = TCPMessageSend(_socket, m_sendBuffer, size);
	if ((result == SOCKET_ERROR) || (result == 0))
	{
		printf("DEBUG// send is incorrect\n");

		int error = WSAGetLastError();
		return (error == WSAESHUTDOWN || error == WSAEDISCON) ? SHUTDOWN : CONNECT_ERROR;
	}

	return SUCCESS;
}
void Host::HandleReturnValue(int _returnValue)
{
	switch (_returnValue)
	{
	case SUCCESS:
		break;
	case ADDRESS_ERROR:
	case BIND_ERROR:
	case CONNECT_ERROR:
	case DISCONNECT:
	case MESSAGE_ERROR:
	case PARAMETER_ERROR:
	case SETUP_ERROR:
	case SHUTDOWN:
	case STARTUP_ERROR:
	{
		CloseSockets();
	}
	break;
	}
}
#pragma endregion

#pragma region Private Functionality
int Host::TCPMessageReceive(SOCKET _socket, char* buf, int len)
{
	int total = 0;

	do
	{
		int ret = recv(_socket, buf + total, len - total, 0);
		if (ret < 1)
			return ret;
		else
			total += ret;

	} while (total < len);

	return total;
}
int Host::TCPMessageSend(SOCKET _socket, const char* data, short length)
{
	int result;
	int bytesSent = 0;

	while (bytesSent < length)
	{
		result = send(_socket, (const char*)data + bytesSent, length - bytesSent, 0);

		if (result <= 0)
			return result;

		bytesSent += result;
	}

	return bytesSent;
}
#pragma endregion

#pragma region Denitialization
Host::~Host()
{
	delete[] m_userName;
	m_userName = nullptr;

	// Terminates use of the WS2_32.DLL
	WSACleanup();
}
#pragma endregion
