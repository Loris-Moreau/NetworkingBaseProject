#include <iostream>
#include <raylib.h>
#include <SDL.h>
#include <SDL_net.h>
#include <string>
#include <vector>

using namespace std;

struct Message
{
    string userName = "idiot";
    bool fromMe = false;
    string content;
};

string GetUserName()
{
    string _name = "Anon";
    return _name;
}

int main(int argc, char* argv[])
{
    // Initialize SDL_net
    if (SDLNet_Init() == -1)
    {
        cerr << "SDLNet_Init error : " << SDLNet_GetError() << '\n';
        return 1;
    }

    // Resolve the server's IP address and port
    IPaddress ip;
    if (SDLNet_ResolveHost(&ip, "localhost" , 4242) == -1)
    {
        cerr << "Resolve Host error : " << SDLNet_GetError() << '\n';
        SDLNet_Quit();
        return 1;
    }

    // Open a TCP connection to the server
    const TCPsocket clientSocket = SDLNet_TCP_Open(&ip);
    if (!clientSocket)
    {
        cerr << "TCP Open error : " << SDLNet_GetError() << '\n';
        SDLNet_Quit();
        return 1;
    }

    // Initialize the window
    constexpr int width = 500, height = 750;
    InitWindow(width, height, "Chat Client");
    SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));

    // Vector to store chat log
    vector<Message> log{Message{GetUserName(), false, "Waiting for someone to talk to..."}};

    // String to store user input
    string typing;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(GRAY);

        // Draw UI elements
        DrawText("Welcome to ChArtFX!", 220, 15, 25, WHITE);
        DrawRectangle(20, 50, width - 40, height - 150, DARKGRAY);
        DrawRectangle(20, height - 90, width - 40, 50, LIGHTGRAY);

        // Draw chat log
        for (int msg = 0; msg < log.size(); msg++)
        {
            DrawText(log[msg].content.c_str(), 30, 75 + (msg * 30), 15, log[msg].fromMe ? SKYBLUE : PURPLE);
        }

        // Draw user input
        DrawText(typing.c_str(), 30, height - 75, 25, DARKBLUE);

        EndDrawing();

        // Handle user input
        int inputChar = GetCharPressed();
        if (inputChar != 0) // A character is pressed on the keyboard
        {
            typing += static_cast<char>(inputChar);
        }
        if (!typing.empty())
        {
            if (IsKeyPressed(KEY_BACKSPACE)) typing.pop_back();
            else if (IsKeyPressed(KEY_ENTER))
            {
                // Send the message typing to the server here!
                string message = typing;

                const int bytesSent = SDLNet_TCP_Send(clientSocket, message.c_str(), message.length() + 1);
                if (bytesSent < message.length() + 1)
                {
                    cerr << "SDLNet TCP Send error : " << SDLNet_GetError() << '\n';
                    SDLNet_TCP_Close(clientSocket);
                    SDLNet_Quit();
                    return 1;
                }

                cout << "Sent " << bytesSent << " bytes to the server" << '\n';

                SDLNet_SocketSet socketSet = SDLNet_AllocSocketSet(1);
                SDLNet_AddSocket(socketSet, reinterpret_cast<SDLNet_GenericSocket>(clientSocket));
                if (SDLNet_CheckSockets(socketSet, 0) != 0)
                {
                    char buffer[1024];
                    int bytesRead = SDLNet_TCP_Recv(clientSocket, buffer, sizeof(buffer));
                    if (bytesRead <= 0)
                    {
                        cerr << "SDLNet TCP Recv error : " << SDLNet_GetError() << '\n';
                        SDLNet_TCP_Close(clientSocket);
                        SDLNet_Quit();
                        return 1;
                    }

                    cout << "Incoming response : " << buffer << '\n';
                }
                log.push_back(Message{GetUserName(), true, typing});
                typing.clear();
            }
        }
    }

    // Close the window and quit SDL_net
    CloseWindow();
    SDLNet_TCP_Close(clientSocket);
    SDLNet_Quit();
    return 0;
}
