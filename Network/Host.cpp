#include "Host.h"

#pragma region Initialization
Host::Host(bool _getIPFromUser) : m_comSocket(0)
{
	// Initiates use of WS2_32.DLL by a processand must be called before any network functions!
	WSADATA wsadata;
	WSAStartup(WINSOCK_VERSION, &wsadata);

	// Examples
	// IP (Loopback) - "127.0.0.1"
	// Port - 31337
	if (_getIPFromUser)
	{
		// Get IP address from user
		HelperFunctionality::GetValidIPAddress(m_buffer, UCHAR_MAX);
	}

	// Get port number from user
	constexpr int LOWEST_PORT_NUMBER_AVAILABLE = 1;
	m_portNumber = static_cast<short>(HelperFunctionality::GetUserInt("Please enter a port number", USHRT_MAX, LOWEST_PORT_NUMBER_AVAILABLE));
}
#pragma endregion

#pragma region Public Functionality
int Host::readMessage()
{
	// Variable for how big the message (partial or full) is/will be
	char size = 0;

	// Read just the header to see how big the message will be
	int result = tcp_recv_whole(m_comSocket, (char*)&size, 1);
	if ((result == SOCKET_ERROR) || (result == 0))
	{
		printf("DEBUG// recv is incorrect\n");

		int error = WSAGetLastError();
		return (error == WSAESHUTDOWN || error == WSAEDISCON) ? SHUTDOWN : CONNECT_ERROR;
	}

	// Read the message
	result = tcp_recv_whole(m_comSocket, m_buffer, size);
	if ((result == SOCKET_ERROR) || (result == 0))
	{
		printf("DEBUG// recv is incorrect\n");

		int error = WSAGetLastError();
		return (error == WSAESHUTDOWN || error == WSAEDISCON) ? SHUTDOWN : CONNECT_ERROR;
	}

	// Print it to the console
	std::cout << m_buffer << std::endl;

	return SUCCESS;
}
int Host::sendMessage()
{
	// Reset buffer
	memset(m_buffer, 0, USHRT_MAX);

	// Get user input
	byte size = HelperFunctionality::GetUserString("Say something: ", m_buffer, USHRT_MAX);

	// Send the header so the receiver knows how big the message will be
	int result = tcp_send_whole(m_comSocket, (char*)&size, 1);
	if ((result == SOCKET_ERROR) || (result == 0))
	{
		printf("DEBUG// send is incorrect\n");

		int error = WSAGetLastError();
		return (error == WSAESHUTDOWN || error == WSAEDISCON) ? SHUTDOWN : CONNECT_ERROR;
	}

	// Send the message
	result = tcp_send_whole(m_comSocket, m_buffer, size);
	if ((result == SOCKET_ERROR) || (result == 0))
	{
		printf("DEBUG// send is incorrect\n");

		int error = WSAGetLastError();
		return (error == WSAESHUTDOWN || error == WSAEDISCON) ? SHUTDOWN : CONNECT_ERROR;
	}

	// Notify the user
	std::cout << "Message sent!" << std::endl;

	return SUCCESS;
}
#pragma endregion

#pragma region Private Functionality
int Host::tcp_recv_whole(SOCKET s, char* buf, int len)
{
	int total = 0;

	do
	{
		int ret = recv(s, buf + total, len - total, 0);
		if (ret < 1)
			return ret;
		else
			total += ret;

	} while (total < len);

	return total;
}
int Host::tcp_send_whole(SOCKET skSocket, const char* data, short length)
{
	int result;
	int bytesSent = 0;

	while (bytesSent < length)
	{
		result = send(skSocket, (const char*)data + bytesSent, length - bytesSent, 0);

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
	// Terminates use of the WS2_32.DLL
	WSACleanup();
}
#pragma endregion
