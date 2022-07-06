#include <iostream>
#include <thread>

#include "Client.h"
#include "HelperFunctionality.h"
#include "Host.h"
#include "Server.h"

void MainThread(Host* _host)
{
	while (_host->HostIsConnected())
	{
		_host->Update();
	}
}
void ReadThread(Host* _host)
{
	while (_host->HostIsConnected())
	{
		_host->ReadMess();
	}
}
void SendThread(Host* _host)
{
	while (_host->HostIsConnected())
	{
		_host->SendMess();
	}
}
int main()
{
	constexpr size_t NUMBER_OF_MENU_OPTIONS = 3;
	const char* options[NUMBER_OF_MENU_OPTIONS] =
	{
		"Client",
		"Server",
		"Exit"
	};

	while (true)
	{
		const int hostTypeSelection = HelperFunctionality::GetUserSelection("Would you like to create a Client or Server?", options, NUMBER_OF_MENU_OPTIONS);

		Host* host = nullptr;

		// Creating two console applications for the client and server
		switch (hostTypeSelection - OFF_BY_ONE)
		{
		case 0:
		{
			host = new Client();

			if (host->HostIsConnected())
			{
				bool mainThreadIsInItsFunction = false;

				std::thread readThread(ReadThread, host);
				std::thread sendThread(SendThread, host);

				if (mainThreadIsInItsFunction == false)
				{
					mainThreadIsInItsFunction = true;

					MainThread(host);
				}

				readThread.join();
				sendThread.join();
			}
		}
			break;
		case 1:
		{
			host = new Server();

			while (host->HostIsConnected())
			{
				host->Update();
			}
		}
			break;
		default:
			return 0;
		}

		delete host;
		host = nullptr;

		std::cout << "Host connection was closed, please try again." << std::endl;
		system("pause");
		system("cls");
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

	return 0;
}