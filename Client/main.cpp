#include <iostream>
#include <string.h>
#include <string>
#include <thread>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

using namespace std;

void receiveSocket(SOCKET sock)
{
	char buf[4096];
	while (1)
	{
		ZeroMemory(buf, 4096);
		int bytesReceived = recv(sock, buf, 4096, 0);

		if (bytesReceived > 0)
		{
			cout << "Server> " << string(buf, 0, bytesReceived) << endl;
		}
	}
}

void main()
{
	string ipAddress = "127.0.0.1";				//Ipadress of the server
	int port = 54000;							//Listening port on the server

	//INISIALISASI WINSOCK
	WSAData data;
	WORD ver = MAKEWORD(2, 2);

	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0)
	{
		cerr << "winsock can't start ! " << wsResult << endl;
		return;
	}

	//CREATE SOCK
		/*Socket adalah program untuk berkomunikasi dengan network layer untuk mengirimkan data ke remote manchine
		,Sama seperti di server*/
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	
	if (sock == INVALID_SOCKET)
	{
		cerr << "can't create socket" << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	//FILL IN A HINT STRUCTURE
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);

	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	// CONNECT TO SERVER
	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	
	if (connResult == SOCKET_ERROR)
	{
		cerr << "Can't connect to server " << WSAGetLastError() << endl;
		closesocket(sock);
		WSACleanup();
		return;
	}

	//THREAD TO RECEIVE DATA
	thread receiveThread (receiveSocket, sock);

	//DO WHILE LOOP TO SEND AND RECEIVE DATA
	string userInput;

	do
	{
		//PROMPT THE USER FOR TEXT
		getline(cin, userInput);

		if (userInput.size() > 0)	//Mengecek apakah user mengetik sesuatu
		{
			//SEND THE TEXT
			send(sock, userInput.c_str(), userInput.size() + 1, 0);
			cout << endl;
		}
	} while (userInput.size() > 0);

	//CLOSE EVERYTHING
	closesocket(sock);
	WSACleanup();

	system("pause");
	return;
}
