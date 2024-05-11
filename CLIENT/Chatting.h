#pragma once

#include "Scenes.h"
#include <vector>
#include <raylib.h>
#include <string>
#include <SDL.h>
#include <SDL_net.h>
#include <iostream>

using std::string;
using std::vector;

struct Message
{
    Message(string contentP, string usernameP, bool fromMeP) :
        content(contentP),
        username(usernameP),
        fromMe(fromMeP)
    {}

    string content = "";
    string username = "";
    bool fromMe = false;
};

class Chatting : public Scenes
{
public:
    Chatting(string ipAdressP, string portP, string usernameP);
    ~Chatting();
    
    bool init();
    
    void draw() override;
    void input() override;
    void update() override;
    void send(const string& message);
    void close();
    
private:
    vector<Message> logs{ Message{"Waiting for someone to talk to...", "", false}};
    
    string typing;
    
    const int width = 500, height = 750;
    
    TCPsocket clientSocket;
    
    string ipAdress;
    string port;
    string username;
};
