#include "Login.h"
#include <iostream>
using namespace std;

Login::Login(function<void(Scenes*)> changeScenes)
{
	changeScenesFunction = changeScenes;
}

Login::~Login() = default;

bool Login::init()
{
	InitWindow(width, height, "Chat Window!");
	SetTargetFPS(60);
	
	return true;
}

void Login::draw()
{
	BeginDrawing();
	ClearBackground(GRAY);
	DrawText("Welcome to ChArtFX!", 15, 15, fontSize, WHITE);
	DrawRectangle(20, 50, width - 40, height - 40*2, DARKGRAY);

	// Server ip
	DrawText("Server ip : ", 30, 40 + fontSize, fontSize, LIGHTGRAY);
	DrawRectangle(30, 40 + fontSize*2 + margin, buttonWidth, buttonHeight, LIGHTGRAY);

	// Server port
	DrawText("Server port : ", 30, 80 + fontSize * 3 + margin, fontSize, LIGHTGRAY);
	DrawRectangle(30, 80 + fontSize * 4 + margin * 2, buttonWidth, buttonHeight, LIGHTGRAY);

	// Username
	DrawText("Username : ", 30, 120 + fontSize * 5 + margin * 2, fontSize, LIGHTGRAY);
	DrawRectangle(30, 120 + fontSize * 6 + margin * 3, buttonWidth, buttonHeight, LIGHTGRAY);

	DrawRectangle(165, 155 + fontSize * 7 + margin * 4, 65, 25, LIGHTGRAY);
	DrawText("Connect", 170, 160 + fontSize * 7 + margin * 4, 15, DARKGRAY);

	DrawText(ipAdress.c_str(), 30 + margin, 40 + fontSize * 2 + margin * 2, fontSize, DARKBLUE);
	DrawText(port.c_str(), 30 + margin, 80 + fontSize * 4 + margin * 3, fontSize, DARKBLUE);
	DrawText(username.c_str(), 30 + margin, 120 + fontSize * 6 + margin * 4, fontSize, DARKBLUE);
	
	EndDrawing();
}

void Login::input()
{
	if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
	{
		const int mouseX = GetMouseX();
		const int mouseY = GetMouseY();

		if (mouseX > 30 && mouseX < 30 + buttonWidth && mouseY > 40 + fontSize * 2 + margin && mouseY < 40 + fontSize * 2 + margin + buttonHeight)
		{
			isTypingIp = true;
			isTypingPort = false;
			isTypingUsername = false;
		}
		else if (mouseX > 30 && mouseX < 30 + buttonWidth && mouseY > 80 + fontSize * 4 + margin * 2 && mouseY < 80 + fontSize * 4 + margin * 2 + buttonHeight)
		{
			isTypingIp = false;
			isTypingPort = true;
			isTypingUsername = false;
		}
		else if (mouseX > 30 && mouseX < 30 + buttonWidth && mouseY > 120 + fontSize * 6 + margin * 3 && mouseY < 120 + fontSize * 6 + margin * 3 + buttonHeight)
		{
			isTypingIp = false;
			isTypingPort = false;
			isTypingUsername = true;
		}

		else if (mouseX > 165 && mouseX < 165 + 65 && mouseY > 155 + fontSize * 7 + margin * 4 && mouseY < 155 + fontSize * 7 + margin * 4 + 25)
		{
			if (ipAdress.size() > 0 && port.size() > 0 && username.size() > 0)
			{
				cout << "Trying to connect\n";

				changeScenesFunction(new Chatting(ipAdress, port, username));
			}
		}
		else
		{
			isTypingIp = false;
			isTypingPort = false;
			isTypingUsername = false;
		}
	}

	const int inputChar = GetCharPressed();
	if (inputChar != 0) //A character is pressed on the keyboard
	{
		if(isTypingIp) ipAdress += static_cast<char>(inputChar);
		if(isTypingPort) port += static_cast<char>(inputChar);
		if(isTypingUsername) username += static_cast<char>(inputChar);
	}
	if (ipAdress.size() > 0 && isTypingIp) if (IsKeyPressed(KEY_BACKSPACE)) ipAdress.pop_back();
	if (port.size() > 0 && isTypingPort) if (IsKeyPressed(KEY_BACKSPACE)) port.pop_back();
	if (username.size() > 0 && isTypingUsername) if (IsKeyPressed(KEY_BACKSPACE)) username.pop_back();
}

void Login::update() {}

void Login::close()
{
	CloseWindow();
}
