#include <iostream>
#include <raylib.h>
#include <SDL.h>
#include <SDL_net.h>
#include <string>
#include <vector>
//#include <string>
using namespace std;

struct Message
{
	bool fromMe = false;
	string content;
};

void initWindow()
{
	vector<Message> log{Message{false, "Waiting for someone to talk to..."}};
	
	const int width = 500, height = 750;
	InitWindow(width, height, "mySpace V2");
	//SetTargetFPS(60);
	SetTargetFPS(GetMonitorRefreshRate(GetCurrentMonitor()));
	
	string typing;
	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(GRAY);
		DrawText("Welcome to ChArtFX!", 220, 15, 25, WHITE);
		DrawRectangle(20, 50, width-40, height -150, DARKGRAY);
		DrawRectangle(20, height - 90, width-40, 50, LIGHTGRAY);
		
		DrawRectangle(20, 50, width-40, height -150, DARKGRAY);

		for(int msg = 0; msg < log.size(); msg++)
		{
			DrawText(log[msg].content.c_str(), 30, 75 + (msg*30), 15, log[msg].fromMe?SKYBLUE:PURPLE);
		}
		
		DrawRectangle(20, height - 90, width-40, 50, LIGHTGRAY);
		int inputChar = GetCharPressed();
		if(inputChar != 0) //A character is pressed on the keyboard
		{
			typing += static_cast<char>(inputChar);
		}
		if(typing.size() > 0)
		{
			DrawText(typing.c_str(), 30, height - 75, 25, DARKBLUE);
		}
		if(typing.size() > 0)
		{
			if(IsKeyPressed(KEY_BACKSPACE)) typing.pop_back();
			else if (IsKeyPressed(KEY_ENTER))
			{
				//Send the message typing to the server here!
				log.push_back(Message{true, typing});
				typing.clear();
			}
			DrawText(typing.c_str(), 30, height - 75, 25, DARKBLUE);
		}
		EndDrawing();
	}
	CloseWindow();
}

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
	
	initWindow();
	
	string message;
	/*cout << "Message : ";
	getline(cin, message);*/
	
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
