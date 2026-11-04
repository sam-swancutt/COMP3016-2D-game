#pragma once
#include <SDL3/SDL.h>

class Assets {
public:
    static void load(SDL_Renderer* renderer);   // loads every sprite once
    static void unload();                       // frees them

    static SDL_Texture* player;
    static SDL_Texture* enemy;
    static SDL_Texture* boss;
    static SDL_Texture* potion;
    static SDL_Texture* sword;
    static SDL_Texture* background;

private:
    static SDL_Texture* loadBMP(SDL_Renderer* renderer, const char* path, bool transparent = true);
};