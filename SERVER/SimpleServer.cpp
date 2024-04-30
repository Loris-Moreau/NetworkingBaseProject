#include <iostream>
#include <string>
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
    SDLNet_SocketSet set = SDLNet_AllocSocketSet(8);
    if (!set)
    {
        cerr << "Socket set allocation error : " << SDLNet_GetError() << '\n';
        SDLNet_Quit();
        return 1;
    }

    while (true)
    {
        TCPsocket clientSocket = SDLNet_TCP_Accept(serverSocket);

        if (clientSocket)
        {
            IPaddress* clientIP = SDLNet_TCP_GetPeerAddress(clientSocket);
            Uint32 ipAddress = SDLNet_Read32(clientIP);
            cout << "Client " << ipAddress << " reached the server" << '\n';

            SDLNet_AddSocket(set, reinterpret_cast<SDLNet_GenericSocket>(clientSocket));

            char buffer[1024];

            if (SDLNet_CheckSockets(set, 10) != 0) // Timeout of 10ms
            { 
                int bytesRead = SDLNet_TCP_Recv(clientSocket, buffer, sizeof(buffer));
                if (bytesRead > 0)
                {
                    string answer;
                    cout << "Incoming message : " << buffer << '\n';
                    if (bytesRead == 7)
                    {
                        answer = "Doom Bot - status : activated";
                    }
                    else if (buffer == "Hello there")
                    {
                        answer = "General Kenobi";
                    }
                    else
                    {
                        answer = "Stop";
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
    }

    SDLNet_FreeSocketSet(set);
    SDLNet_Quit();
    return 0;
}
