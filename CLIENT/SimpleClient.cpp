#include "Chatting.h"
#include "Login.h"
#include <SDL.h>
#include <SDL_net.h>
#include <raylib.h>
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
using namespace std;

void changeScene(Scenes* scenes);

//ChatScene* chat = new ChatScene();ChatScene* chat = new ChatScene();ChatScene* chat = new ChatScene();
Scenes* currentScene = new Login(changeScene);;

int main(int argc, char* argv[])
{
    if (currentScene->init())
    {
        cout << "Login initialized" << '\n';
        
        while (!WindowShouldClose())
        {
            currentScene->input();
            currentScene->update();
            currentScene->draw();
        }
        currentScene->close();
    }
    else
    {
        cerr << "Scene failed to initialize" << '\n';
    }
    return 0;
}

void changeScene(Scenes* scenes)
{
    currentScene->close();
    currentScene = scenes;
    currentScene->init();
    
    cout << "Scene change" << '\n';
}
