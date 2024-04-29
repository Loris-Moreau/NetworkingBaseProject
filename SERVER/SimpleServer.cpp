#include <iostream>
#include <regex>
#include <SDL.h>
#include <SDL_net.h>
using namespace std;

int main(int argc, char* argv[])
{
	if (SDLNet_Init() == -1)
	{
		cerr << "SDLNet_Init error : " << SDLNet_GetError() << '\n';
		return 1;
	}
	IPaddress ip;
	if (SDLNet_ResolveHost(&ip, nullptr, 4242) == -1)
	{
		cerr << "Resolve Host error : " << SDLNet_GetError() << '\n';
		SDLNet_Quit();
		return 1;
	}
	const TCPsocket serverSocket = SDLNet_TCP_Open(&ip);
	if (!serverSocket)
	{
		cerr << "TCP Open error : " << SDLNet_GetError() << '\n';
		SDLNet_Quit();
		return 1;
	}
	
	TCPsocket clientSocket;
	while (true)
	{
		clientSocket = SDLNet_TCP_Accept(serverSocket);
		if (clientSocket)
		{
			cout << "A client reached the server" << '\n';
			char buffer[1024];
			int bytesRead = SDLNet_TCP_Recv(clientSocket, buffer, sizeof(buffer));
			if (bytesRead > 0)
			{
				string answer;
				cout << "Incoming message : " << buffer << '\n';
				if(bytesRead == 7)
				{
					answer = "Doom Bot - status : activated";
				}
				else if(strcmp(buffer, "Hello there") != 0)
				{
					answer = "General Kenobi";
				}
				else
				{
					answer = "Stop bothering me";
				}
				const int bytesSent = SDLNet_TCP_Send(clientSocket, answer.c_str(), answer.length() + 1);
				if (bytesSent < answer.length() + 1)
				{
					cerr << "SDLNet TCP Send error : " << SDLNet_GetError() << '\n';
					break;
				}
			}
		}
	}
	
	return 0;
}
