#ifndef OBJECT_H
#define OBJECT_H

#define _ZOOM_OUT_ 4
#define _SPEED_ 20
#define _SPEED_PROJECTILE_ 10
#define _HEAL_ 3;
#define _BOUNCE_ 3;

//ms
#define _COOLDOWN_ 200

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

enum class ItemType{

    Life,
    Shield,
    Time

};

struct Player{
    SDL_Texture * texture = nullptr;
    SDL_FPoint position = {0,0};
    int width = 0;
    int height = 0;
    int speed = _SPEED_;
    int currentHealth = 999;
    Uint32 coolDown = _COOLDOWN_;//ms
    Uint32 lastShootTime = 0;


    
};

struct ProjectilePlayer{
    SDL_Texture * texture = nullptr;
    SDL_FPoint position = {0,0};
    int width = 0;
    int height = 0;
    int speed = _SPEED_ + _SPEED_PROJECTILE_;
    int damage = 1;

};

struct Enemy{
    SDL_Texture * texture = nullptr;
    SDL_FPoint position = {0,0};
    SDL_FPoint direction = {0,0};
    int width = 0;
    int height = 0;
    int speed = _SPEED_;
    int currentHealth = 2;
    Uint32 coolDown = _COOLDOWN_;//ms
    Uint32 lastShootTime = 0;
};

struct ProjectileEnemy{
    SDL_Texture * texture = nullptr;
    SDL_FPoint position = {0,0};
    SDL_FPoint direction = {0,0};
    int width = 0;
    int height = 0;
    int speed = _SPEED_ + _SPEED_PROJECTILE_;
    int damage = 1;
};

struct Explosion{
    SDL_Texture * texture = nullptr;
    SDL_FPoint position = {0,0};
    int width = 0;
    int height = 0;
    int currentFrame = 0;
    int totalFrame = 0;
    Uint32 startTime = 0;
    Uint32 FPS = 10;
};

struct Item{
    SDL_Texture * texture = nullptr;
    SDL_FPoint position = {0,0};
    SDL_FPoint direction = {0,0};
    int width = 0;
    int height = 0;
    ItemType type = ItemType::Life;
    int heal = _HEAL_;
    int speed = _SPEED_ -10;

    int bounce = _BOUNCE_;
};

#endif