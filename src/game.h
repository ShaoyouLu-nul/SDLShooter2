#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include "scene.h"

class Game{
private:
    Game();
    Game(const Game & game) = delete;
    Game& operator=(const Game & game) = delete;
    
public:
    ~Game();
    static Game& getInstance()
    {
        static Game game;
        return game;
    }
    void init();
    void run();
    void clean();

    void handleEvent(SDL_Event * event);
    void update(float deltaTime);
    void render();
    
    void changeScene(Scene * scene);

    SDL_Window * getWindow(){return window;}
    SDL_Renderer * getRenderer(){return renderer;}
    int getWindowWidth(){return windowWidth;}
    int getWindowHeight(){return windowHeight;}
    int getFPS(){return FPS;}

private:
    bool isRunning = true;
    Scene * currentScene = nullptr;
    SDL_Window * window = nullptr;
    SDL_Renderer * renderer = nullptr;
    int windowWidth = 600;
    int windowHeight = 800;
    int FPS = 60;
    Uint32 frameTime;
    float deltaTime;
    

};


#endif
