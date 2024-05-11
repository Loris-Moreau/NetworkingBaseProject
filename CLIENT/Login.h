#pragma once
#include "Scenes.h"
#include "Chatting.h"
#include <vector>
#include <raylib.h>
#include <string>
#include <SDL.h>
#include <SDL_net.h>
#include <iostream>
#include <functional>

using std::string;
using std::function;

class Login : public Scenes
{
public:
    Login(function<void(Scenes*)>changeScenes);
    ~Login();
    
    bool init() override;
    
    void draw() override;
    void input() override;
    void update() override;
    void close() override;

private:
    function<void(Scenes*)> changeScenesFunction;
    string typing;
    
    const int width = 400, height = 400;
    
    string ipAdress;
    string port;
    string username;

    bool isTypingIp = false;
    bool isTypingPort = false;
    bool isTypingUsername = false;

    const int fontSize = 20;
    const int margin = 10;
    const int buttonHeight = 50;
    const int buttonWidth = width - 80;
};
