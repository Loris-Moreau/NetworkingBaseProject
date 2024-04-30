#include <iostream>
#include <SDL.h>
#include <SDL_net.h>
using namespace std;

int main(int argc, char* argv[])
{
    if (SDLNet_Init() == -1)
    {
        cerr << "SDLNet Initialization error : " << SDLNet_GetError() << '\n';
        return 1;
    }

    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, nullptr, 4242) == -1)
    {
        cerr << "Resolve Host error : " << SDLNet_GetError() << '\n';
        SDLNet_Quit();
        return 1;
    }

    TCPsocket serverSocket = SDLNet_TCP_Open(&ip);
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
            cout << "A client joined" << '\n';
            while (true)
            {
                char buffer[1024];
                int _bytesRead = SDLNet_TCP_Recv(clientSocket, buffer, sizeof(buffer));
                if (_bytesRead > 0)
                {
                    cout << "Incoming message : " << buffer << '\n';
                    string answer = "Message received";
                    int _bytesSent = SDLNet_TCP_Send(clientSocket, answer.c_str(), answer.length() + 1);
                    if (_bytesSent < answer.length() + 1)
                    {
                        cerr << "SDLNet TCP Send error : " << SDLNet_GetError() << '\n';
                        break;
                    }
                }
                else
                {
                    cout << "Client disconnected" << '\n';
                    break;
                }
            }
            SDLNet_TCP_Close(clientSocket);
        }
    }

    SDLNet_TCP_Close(serverSocket);
    SDLNet_Quit();
    return 0;
}
