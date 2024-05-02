#include <iostream>
#include <mutex>
#include <SDL.h>
#include <SDL_net.h>
#include <thread>
#include <vector>
using namespace std;

struct UserData
{
    TCPsocket socket;
    string name;
};

vector<UserData> clients;
SDLNet_SocketSet set = SDLNet_AllocSocketSet(8);	//Put the maximum number of sockets
//mutex mtx;

void HandleClient(TCPsocket clientSocket)
{
    char nameBuffer[1024];
    int bytesReadName = SDLNet_TCP_Recv(clientSocket, nameBuffer, sizeof(nameBuffer));
    string userName(nameBuffer);
    if (bytesReadName > 0)
    {
        cout << "User " << userName << " has joined the server\n";
    }

    char buffer[1024];
    while (true)
    {
        if(SDLNet_CheckSockets(set, 0)!=0)	//Here, 0 is the timeout
        {
            int bytesRead = SDLNet_TCP_Recv(clientSocket, buffer, sizeof(buffer));
            if (bytesRead > 0)
            {
                cout << userName << " : " << buffer << '\n';

                // Broadcast the message to all clients
                string message = string(buffer);
                {
                    //lock_guard<mutex> lock(mtx);
                    for (UserData& client : clients)
                    {
                        if (client.socket != clientSocket)
                        {
                            int bytesSent = SDLNet_TCP_Send(client.socket, message.c_str(), message.length() + 1);
                            if (bytesSent < message.length() + 1)
                            {
                                cerr << "SDLNet TCP Send error: " << SDLNet_GetError() << '\n';
                            }
                        }
                    }
                
                }
            }
        }
        else
        {
            SDLNet_TCP_Close(clientSocket);
            {
                //lock_guard<mutex> lock(mtx);
                clients.erase(remove_if(clients.begin(), clients.end(),
                [clientSocket](const UserData& client)
                { return client.socket == clientSocket;}), clients.end());
            }
            cout << userName << " has disconnected." << '\n';
            break;
        }
    }
}

int main(int argc, char* argv[])
{
    if (SDLNet_Init() == -1)
    {
        cerr << " SDLNet Initialization error: " << SDLNet_GetError() << '\n';
        return 1;
    }

    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, nullptr, 4242) == -1)
    {
        cerr << " Resolve Host error: " << SDLNet_GetError() << '\n';
        SDLNet_Quit();
        return 1;
    }

    TCPsocket serverSocket = SDLNet_TCP_Open(&ip);
    if (!serverSocket)
    {
        cerr << " TCP Open error: " << SDLNet_GetError() << '\n';
        SDLNet_Quit();
        return 1;
    }

    while (true)
    {
        TCPsocket clientSocket = SDLNet_TCP_Accept(serverSocket);
        if (clientSocket)
        {
            //thread(HandleClient, clientSocket).detach();
            UserData userData;
            userData.socket = clientSocket;
            {
                //lock_guard<mutex> lock(mtx);
                clients.push_back(userData);
            }
            SDLNet_AddSocket(set, reinterpret_cast<SDLNet_GenericSocket>(clientSocket));

        }
    }

    SDLNet_TCP_Close(serverSocket);
    SDLNet_Quit();
    return 0;
}
