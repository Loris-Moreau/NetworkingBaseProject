
#include <iostream>
#include <SDL.h>
#include <SDL_net.h>
//#include <string>
using namespace std;

int main(int argc, char* argv[])
{
	if (SDLNet_Init() == -1)
	{
		cerr << "SDLNet_Init error : " << SDLNet_GetError() << '\n';
		return 1;
	}
	IPaddress ip;
	if (SDLNet_ResolveHost(&ip, "localhost" , 4242) == -1)
	{
		cerr << "Resolve Host error : " << SDLNet_GetError() << '\n';
		SDLNet_Quit();
		return 1;
	}
	const TCPsocket clientSocket= SDLNet_TCP_Open(&ip);
	if (!clientSocket)
	{
		cerr << "TCP Open error : " << SDLNet_GetError() << '\n';
		SDLNet_Quit();
		return 1;
	}
	string message;
	cout << "Message : ";
	cin >> message;
	
	const int bytesSent = SDLNet_TCP_Send(clientSocket, message.c_str(), message.length() + 1);
	if (bytesSent < message.length() + 1)
	{
		cerr << "SDLNet TCP Send error : " << SDLNet_GetError() << '\n';
		SDLNet_TCP_Close(clientSocket);
		SDLNet_Quit();
		return 1;
	}
	
	cout << "Sent " << bytesSent << " bytes to the server" << '\n';

	char buffer[1024];
	const int bytesRead = SDLNet_TCP_Recv(clientSocket, buffer, sizeof(buffer));
	if (bytesRead <= 0)
	{
		cerr << "SDLNet TCP Recv error : " << SDLNet_GetError() << '\n';
		SDLNet_TCP_Close(clientSocket);
		SDLNet_Quit();
		return 1;
	}
	
	cout << "Incoming response : " << buffer << '\n';
	
	//SDLNet_TCP_Close(clientSocket);
	//SDLNet_Quit();
	
	return 0;
}
