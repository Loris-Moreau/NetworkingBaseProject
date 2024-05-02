#include <iostream>
#include <SDL.h>
#include <SDL_net.h>
#include <string>
#include <vector>
#include "raylib.h"
using namespace std;

struct Message
{
    string sender;
    string content;
};

int main(int argc, char* argv[])
{
    constexpr int width = 400, height = 300;
    InitWindow(width, height, "Login");
    SetTargetFPS(60);

    string host = "localhost";
    string s_port;
    string _name;
    
    int port = 4242;
    bool nameComplete = false;
    bool portComplete = false;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(DARKGRAY);
        DrawText("Login ", 155, 10, 40, WHITE); // Login Text
        DrawText("Server : ", 10, 90, 25, WHITE); // Server Text
        DrawText(host.c_str(), 130, 90, 25, WHITE); // Host Text
        DrawText("port ", 10, 160, 25, WHITE); // Port
        DrawText("name ", 10, 230, 25, WHITE); // Name

        DrawRectangle(70, 160, width - 100, 25, WHITE);
        DrawRectangle(70, 230, width - 100, 25, WHITE);

        int inputChar = GetCharPressed();
        if (inputChar != 0)
        {
            if (!portComplete)
            {
                s_port += static_cast<char>(inputChar);
            }
            else if (!nameComplete)
            {
                _name += static_cast<char>(inputChar);
            }
        }
        if (IsKeyPressed(KEY_ENTER))
        {
            if (!portComplete)
            {
                port = stoi(s_port);
                portComplete = true;
            }
            else if (!_name.empty())
            {
                nameComplete = true;
                break;
            }
        }
        else if (IsKeyPressed(KEY_BACKSPACE))
        {
            if (portComplete && !s_port.empty())
            {
                s_port.pop_back();
            }
        }
        if (!s_port.empty())
        {
            DrawText(s_port.c_str(), 75, 160, 25, BLACK);
        }
        if (!_name.empty())
        {
            DrawText(_name.c_str(), 75, 230, 25, BLACK);
        }
        EndDrawing();
    }
    CloseWindow();

    if (SDLNet_Init() == -1)
    {
        cerr << "SDLNet_Init error: " << SDLNet_GetError() << '\n';
        return 1;
    }

    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, host.c_str(), port) == -1)
    {
        cerr << "Resolver Host error: " << SDLNet_GetError() << '\n';
        return 1;
    }

    TCPsocket clientSocket = SDLNet_TCP_Open(&ip);
    if (!clientSocket)
    {
        cerr << "TCP Open error: " << SDLNet_GetError() << '\n';
        SDLNet_Quit();
        return 1;
    }

    SDLNet_SocketSet clientSocketSet = SDLNet_AllocSocketSet(1); 
    if (!clientSocketSet)
    {
        cerr << "SDLNet_AllocSocketSet error: " << SDLNet_GetError() << '\n';
        SDLNet_TCP_Close(clientSocket);
        SDLNet_Quit();
        return 1;
    }

    if (SDLNet_TCP_AddSocket(clientSocketSet, clientSocket) == -1)
    {
        cerr << "SDLNet_TCP_AddSocket error: " << SDLNet_GetError() << '\n';
        SDLNet_TCP_Close(clientSocket);
        SDLNet_Quit();
        return 1;
    }

    string typing;

    // UserName Sending
    int bytesSent = SDLNet_TCP_Send(clientSocket, _name.c_str(), _name.length() + 1);
    if (bytesSent < _name.length() + 1)
    {
        cerr << "SDLNet TCP Send error : " << SDLNet_GetError() << '\n';
        SDLNet_TCP_Close(clientSocket);
        SDLNet_Quit();
        return 1;
    }

    const int width2 = 500, height2 = 750;
    InitWindow(width2, height2, "Chat");
    SetTargetFPS(60);

    vector<Message> log{Message{"Welcome to Hell"}};

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(GRAY);
        DrawText(TextFormat("Welcome ", _name.c_str()), 150, 15, 25, WHITE);
        DrawRectangle(20, 50, width2 - 40, height2 - 150, DARKGRAY);
        DrawRectangle(20, height2 - 90, width2 - 40, 50, WHITE);

        int inputChar = GetCharPressed();
        if (inputChar != 0)
        {
            typing += static_cast<char>(inputChar);
        }
        if (!typing.empty())
        {
            if (IsKeyPressed(KEY_BACKSPACE))
            {
                typing.pop_back();
            }
            else if (IsKeyPressed(KEY_ENTER))
            {
                // Send message to the server
                int bytesSent = SDLNet_TCP_Send(clientSocket, typing.c_str(), typing.length() + 1);
                if (bytesSent < typing.length() + 1)
                {
                    cerr << "SDLNet TCP Send error: " << SDLNet_GetError() << '\n';
                    SDLNet_TCP_Close(clientSocket);
                    SDLNet_Quit();
                    return 1;
                }
                log.push_back(Message{ _name, typing });
                typing.clear();
            }
            DrawText(typing.c_str(), 30, height2 - 75, 25, BLACK);
        }

        // Client Messages
        if (SDLNet_CheckSockets(clientSocketSet, 0) > 0 && SDLNet_SocketReady(clientSocket))
        {
            char buffer[1024];
            int bytesRead = SDLNet_TCP_Recv(clientSocket, buffer, sizeof(buffer));
            if (bytesRead > 0)
            {
                string receivedMessage(buffer);
                log.push_back(Message{ _name, receivedMessage });
            }
            else
            {
                cerr << _name <<" Disconnected" << '\n';
                break;
            }
        }

        for (size_t i = 0; i < log.size(); i++)
        {
            DrawText(TextFormat("[%s] %s", log[i].sender.c_str(), log[i].content.c_str()), 30, 75 + (i * 30), 15, BLUE);
        }
        EndDrawing();
    }
    CloseWindow();

    SDLNet_TCP_Close(clientSocket);
    SDLNet_FreeSocketSet(clientSocketSet); 
    SDLNet_Quit();
    
    return 0;
}
