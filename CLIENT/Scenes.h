#pragma once

class Scenes
{
public:
    Scenes();
    virtual ~Scenes();
    
    virtual bool init();
    
    virtual void draw();
    virtual void input();
    virtual void update();
    virtual void close();
};
