#include "Assets.h"

SDL_Texture* Assets::player = nullptr;
SDL_Texture* Assets::enemy = nullptr;
SDL_Texture* Assets::boss = nullptr;

SDL_Texture* Assets::loadBMP(SDL_Renderer* renderer, const char* path) {
    SDL_Surface* surface = SDL_LoadBMP(path);
    if (!surface) {
        SDL_Log("Could not load %s: %s", path, SDL_GetError());
        return nullptr;                 // the game falls back to a coloured square
    }

    // Magenta pixels become see-through
    SDL_SetSurfaceColorKey(surface, true, SDL_MapSurfaceRGB(surface, 255, 0, 255));

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);

    if (texture) {
        SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);   // keeps pixel art crisp
    }
    return texture;
}

void Assets::load(SDL_Renderer* renderer) {
    player = loadBMP(renderer, "assets/player.bmp");
    enemy = loadBMP(renderer, "assets/enemy.bmp");
    boss = loadBMP(renderer, "assets/boss.bmp");
}

void Assets::unload() {
    if (player) { SDL_DestroyTexture(player); player = nullptr; }
    if (enemy) { SDL_DestroyTexture(enemy);  enemy = nullptr; }
    if (boss) { SDL_DestroyTexture(boss);   boss = nullptr; }
}