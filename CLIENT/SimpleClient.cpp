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
    string _host = "localhost";
    string _name;
    string _sPort;
    int _port = 4242;
    
    bool _hostComplete = false;
    bool _nameComplete = false;
    bool _portComplete = false;

    ////////// Login Window //////////
    constexpr int width = 400, height = 300;
    InitWindow(width, height, "Login Window");
    SetTargetFPS(60);
    
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(DARKGRAY);
        DrawText("Login", 155, 10, 40, WHITE); // Login Text
        DrawText("IP", 25, 90, 25, WHITE);    // Ip Address
        DrawText("Port", 10, 160, 25, WHITE); // Port
        DrawText("name", 10, 230, 25, WHITE); // User Name

        DrawRectangle(70, 90, width - 100, 25, WHITE);
        DrawRectangle(70, 160, width - 100, 25, WHITE);
        DrawRectangle(70, 230, width - 100, 25, WHITE);

        int _inputChar = GetCharPressed();
        
        if (_inputChar != 0)
        {
            /*if (!_hostComplete)
            {
                _host += static_cast<char>(_inputChar);
            }
            else*/ if (!_portComplete)
            {
                _sPort += static_cast<char>(_inputChar);
            }
            else if (!_nameComplete)
            {
                _name += static_cast<char>(_inputChar);
            }
        }
        if (IsKeyPressed(KEY_ENTER))
        {
            /*if (!_hostComplete)
            {
                _hostComplete = true;
            }
            else */if (!_portComplete)
            {
                _port = stoi(_sPort);
                _portComplete = true;
            }
            else if (!_name.empty())
            {
                _nameComplete = true;
                break;
            }
        }
        else if (IsKeyPressed(KEY_BACKSPACE))
        {
            if (_hostComplete && !_name.empty())
            {
                _name.pop_back();
            }
            else if (_portComplete && !_sPort.empty())
            {
                _sPort.pop_back();
            }
            else if (!_host.empty())
            {
                _host.pop_back();
            }
        }
        if (!_host.empty())
        {
            DrawText(_host.c_str(), 75, 90, 25, BLACK);
        }
        if (!_sPort.empty())
        {
            DrawText(_sPort.c_str(), 75, 160, 25, BLACK);
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
    if (SDLNet_ResolveHost(&ip, _host.c_str(), _port) == -1)
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
    
    string typing;
    vector<Message> log{Message{_name," has Joined"}};

    // Send the user's name to the server
    int bytesSent = SDLNet_TCP_Send(clientSocket, _name.c_str(), _name.length() + 1);
    if (bytesSent < _name.length() + 1) 
    {
        cerr << "SDLNet TCP Send error : " << SDLNet_GetError() << '\n';
        SDLNet_TCP_Close(clientSocket);
        SDLNet_Quit();
        return 1;
    }

    
    ////////// Main Window //////////
    constexpr int width2 = 500, height2 = 750;
    InitWindow(width2, height2, "mySpace V2");
    SetTargetFPS(60);
    
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(GRAY);
        DrawText("Welcome ", 150, 15, 25, WHITE);
        DrawRectangle(20, 50, width2 - 40, height2 - 150, DARKGRAY);
        DrawRectangle(20, height2 - 90, width2 - 40, 50, WHITE);

        int _inputChar = GetCharPressed();
        if (_inputChar != 0)
        {
            typing += static_cast<char>(_inputChar);
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
                // Adds the message to the log, then clears the typing
                log.push_back(Message{_name, typing});
                typing.clear();
            }
            
            DrawText(typing.c_str(), 30, height2 - 75, 25, BLACK);
        }

        for (size_t i = 0; i < log.size(); i++)
        {
            DrawText(TextFormat("[%s] %s", log[i].sender.c_str(), log[i].content.c_str()), 30, 75 + (i * 30), 15, SKYBLUE);
        }

        EndDrawing();
    }
    CloseWindow();

    SDLNet_TCP_Close(clientSocket);
    SDLNet_Quit();
    return 0;
}
