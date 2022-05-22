#include <iostream>
//Windows Socket -> Untuk mengakses network socket
#include <WS2tcpip.h>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#pragma comment (lib, "ws2_32.lib")

using namespace std;

void main()
{
	//INISIALISASI WINSOCK
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);		//Version 2.2

	int wsock = WSAStartup(ver, &wsData);		//

	if (wsock != 0)
	{
		cerr << "initialize error ! " << endl;
		return;
	}
	else
	{
		cout << "Winsock Ready" << endl;
	}

	//CREATE A SOCK
	//Socket berfungsi untuk mengakses suatu port di ip address
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);			//AF_Inet = version 4

	if (listening == INVALID_SOCKET)
	{
		cerr << "Socket Error ! err# " << endl;
		return;
	}
	else
	{
		cout << " Winsock Ready" << endl;
	}


	//BIND IP ADDRESS AND PORT TO A SOCKET
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);								//htons = host to network short
	hint.sin_addr.S_un.S_addr = INADDR_ANY;						//Bisa menggunakan inet_pton

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	//TELL WINSOCK THE SOCKET FOR LISTENING
	listen(listening, SOMAXCONN);

	fd_set master;
	FD_ZERO(&master);

	FD_SET(listening, &master);

	bool running = true;

	while (running)
	{
		fd_set copy = master;
		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		for (int i = 0; i < socketCount; i++)
		{
			SOCKET sock = copy.fd_array[i];

			if (sock == listening)
			{
				// Accept a new connection
				SOCKET client = accept(listening, nullptr, nullptr);

				// Add the new connection to the list of connected clients
				FD_SET(client, &master);

				// Send a welcome message to the connected client
				string welcomeMsg = "Welcome to Chat\r\n";

				send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);

				// broadcast welcome message
				ostringstream ss;
				ss << "Welcome to server Client" << " #" << client << " connected.\r\n";
				string strOut = ss.str();

				for (int i = 0; i < master.fd_count; i++)
				{
					SOCKET outSock = master.fd_array[i];
					if (outSock != listening && outSock != client)
					{
						send(outSock, strOut.c_str(), strOut.size() + 1, 0);
					}
				}
				// logging welcome message to server
				cout << strOut << endl;
			}
			else
			{
				char buf[4096];
				ZeroMemory(buf, 4096);

				// Receive Message
				int bytesIn = recv(sock, buf, 4096, 0);
				if (bytesIn <= 0)
				{
					// Drop the client
					closesocket(sock);
					FD_CLR(sock, &master);
				}
				else
				{

					// Send message to other client, and definiately not the listening socket
					ostringstream ss;
					ss << "Client #" << sock << ":" << buf << "\r\n";
					string strOut = ss.str();
					for (int i = 0; i < master.fd_count; i++)
					{
						SOCKET outSock = master.fd_array[i];
						if (outSock != listening && sock != outSock)
						{
							send(outSock, strOut.c_str(), strOut.size() + 1, 0);
						}
					}
					// logging user`s message to server
					cout << strOut << endl;

					
				}
			}
		}
	}

	//CLOSE LISTENING SOCKET
	FD_CLR(listening, &master);
	closesocket(listening);

	//WAIT FOR CONNECTION
	WSACleanup();

	system("pause");
	return;


}




















	/*char host[NI_MAXHOST];										// remote name client
	char service[NI_MAXSERV];									// port yang client connected

	ZeroMemory(host, NI_MAXHOST);								//if using linux/IOS = memset(host, 0, NI_MAXHOST)
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		cout << host << " connected to port " << service << endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		cout << host << " connected on port " << ntohs(client.sin_port) << endl;
	}

	// CLOSE LISTENING SOCKET
	closesocket(listening);

	//WHILE LOOP: ACCEPT AND ECHO MESSAGE BACK TO CLIENT
	char buf[4096];

	while (true)
	{
		ZeroMemory(buf, 4096);

		//WAIT FOR CLIENT TO SEND DATA
		int bytesReceived = recv(clientSocket, buf, 4096, 0);

		if (bytesReceived == SOCKET_ERROR)
		{
			cout << "Error in recv(). Quitting " << endl;
			break;
		}

		if (bytesReceived == 0)
		{
			cout << "Client disconnected" << endl;
			break;
		}

		//ECHO MESSAGE BACK TO CLIENT
		send(clientSocket, buf, bytesReceived + 1, 0);
	}

	//CLOSE THE SOCKET
	closesocket(clientSocket);

	//SHUTDOWN WINSOCK
	WSACleanup;
}*/