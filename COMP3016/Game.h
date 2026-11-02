#pragma once
#include <SDL3/SDL.h>
#include <memory>
#include "Player.h"

class Game {
public:
    Game() = default;
    ~Game();

    bool init(const char* title, int width, int height);
    void run();

private:
    void handleEvents();
    void update(float dt);
    void render();
    void shutdown();

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    bool running = false;

    std::unique_ptr<Player> player;
};