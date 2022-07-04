#include <iostream>

#include "Client.h"
#include "HelperFunctionality.h"
#include "Host.h"
#include "Server.h"

enum class HostType { Client = 0, Server, NumberOfTypes };

int main()
{
	const char* options[static_cast<int>(HostType::NumberOfTypes)] =
	{
		"Client",
		"Server",
	};

	const int hostTypeSelection = HelperFunctionality::GetUserSelection("Would you like to create a Client or Server?", options, static_cast<int>(HostType::NumberOfTypes));
	
	// Creating two console applications for the client and server
	Host* host;
	if ((hostTypeSelection - OFF_BY_ONE) == static_cast<int>(HostType::Client))
	{
		host = new Client();
	}
	else
	{
		host = new Server();
	}

	while (true)
	{
		host->Update();
	}

	// Server
	//- Advertise the TCP information over UDP connectiono
	//		- Advertise every 1 second
	//- Connect clients to chat
	//- Excahge messages over over TCP connection
	//		- Respond to commands
	//		- Echo chat messages back to all connected clients
	//		- Display clients' actions (connect, disconnect, register, get list, get log)
	//		- Log all commands and chat messages to a .txt file

	// Client
	//- Listen on UDP to read server's TCP info
	//- Prompt user to choose a username
	//- Establish TCP connection with server
	//- Run commands $register, $exit, $getlist, and $getlog
	//		- User can run $exit without being registered with server
	//			- Other commands permitted only after successful registration
	//- Send and receive chat messages
	//- Display the messages echoed by the server

	delete host;

	return 0;
}