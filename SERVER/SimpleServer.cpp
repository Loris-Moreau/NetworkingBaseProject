#include <iostream>
#include <SDL_net.h>
#include <thread>
#include <vector>
using namespace std;

// handle each client connection
void HandleClient(TCPsocket clientSocket)
{
    char nameBuffer[1024];
    int bytesReadName = SDLNet_TCP_Recv(clientSocket, nameBuffer, sizeof(nameBuffer));
    string clientName(nameBuffer);
    if (bytesReadName > 0)
    {
        cout << "Client " << clientName << " joined\n";
    }
    
    while (true)
        {
        char buffer[1024];
        int bytesRead = SDLNet_TCP_Recv(clientSocket, buffer, sizeof(buffer));
        if (bytesRead > 0)
        {
            cout << clientName << " : " << buffer << '\n';
            string answer = "Message received";
            int bytesSent = SDLNet_TCP_Send(clientSocket, answer.c_str(), answer.length() + 1);
            if (bytesSent < answer.length() + 1)
            {
                cerr << "SDLNet TCP Send error : " << SDLNet_GetError() << '\n';
                break;
            }
        }
        else
        {
            cout << clientName << " disconnected\n";
            break;
        }
    }
    SDLNet_TCP_Close(clientSocket);
}

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

    vector<thread> clientThreads;
    
    while (true)
    {
        TCPsocket clientSocket = SDLNet_TCP_Accept(serverSocket);
        if (clientSocket)
        {
            // Start a new thread to handle the client connection
            clientThreads.emplace_back(HandleClient, clientSocket);
        }
    }

    // Join all client threads
    for (auto& thread : clientThreads)
    {
        thread.join();
    }

    SDLNet_TCP_Close(serverSocket);
    SDLNet_Quit();
    return 0;
}
