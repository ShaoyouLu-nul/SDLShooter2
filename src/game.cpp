#include "game.h"
#include "sceneMain.h"

Game::Game()
{
}

Game::~Game()
{
    clean();
}

void Game::init()
{
    frameTime = 1000/FPS;

   if(SDL_Init(SDL_INIT_EVERYTHING) != 0) 
   {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,"SDL_Init Error: %s",SDL_GetError());
        isRunning = false;
   }

   window = SDL_CreateWindow("SDLshooter",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,windowWidth,windowHeight,SDL_WINDOW_SHOWN);
   if(window == nullptr)
   {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,"create window Error: %s",SDL_GetError());
        isRunning = false;
   }
   renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
   if(renderer == nullptr)
   {
            SDL_LogError(SDL_LOG_CATEGORY_ERROR,"create renderer Error: %s",SDL_GetError());
        isRunning = false;
   }

   if(IMG_Init(IMG_INIT_JPG |  IMG_INIT_PNG)!= (IMG_INIT_JPG |  IMG_INIT_PNG))
   {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR,"IMG_Init Error: %s",IMG_GetError());
        isRunning = false;
   }

   currentScene = new SceneMain;
   currentScene->init();

}

void Game::run()
{
    
    while(isRunning)
    {
        Uint32 frameStart = SDL_GetTicks();
        SDL_Event event;
        handleEvent(&event);
        update(deltaTime);
        render();
        Uint32 frameEnd = SDL_GetTicks();
        Uint32 diff =  frameEnd - frameStart;
        if(diff <frameTime)
        {
            SDL_Delay(diff);    
            deltaTime = frameTime/1000.0f;
        }
        else{
            deltaTime = diff/1000.0f;

        }


    }
}

void Game::clean()
{
    if(currentScene != nullptr)
        delete currentScene;

    IMG_Quit();
    
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    
}

void Game::handleEvent(SDL_Event * event)
{    
    while(SDL_PollEvent(event))
    {
        if(event->type == SDL_QUIT)
        {
            isRunning = false;
            currentScene->handleEvent(event);
        }
    }
}

void Game::update(float deltaTime)
{
    currentScene->update(deltaTime);
}

void Game::render()
{
    SDL_RenderClear(renderer);
    currentScene->render();
    SDL_RenderPresent(renderer);
}

void Game::changeScene(Scene * scene)
{
    if(currentScene != nullptr)
        delete currentScene;
        
    currentScene = scene;
    currentScene->init();

}


