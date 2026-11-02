#include "Game.h"
#include <algorithm>
#include <cmath>

Game::~Game() {
    shutdown();
}

bool Game::init(const char* title, int width, int height) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow(title, width, height, 0);
    if (!window) {
        SDL_Log("Window failed: %s", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        SDL_Log("Renderer failed: %s", SDL_GetError());
        return false;
    }

    player = std::make_unique<Player>(640.0f, 360.0f);

    enemies.push_back(std::make_unique<Enemy>(100.0f, 100.0f, player.get()));
    enemies.push_back(std::make_unique<Enemy>(1100.0f, 100.0f, player.get()));
    enemies.push_back(std::make_unique<Enemy>(600.0f, 650.0f, player.get()));

    return true;
}

void Game::run() {
    running = true;
    Uint64 last = SDL_GetTicks();

    while (running) {
        Uint64 now = SDL_GetTicks();
        float dt = (now - last) / 1000.0f;
        last = now;

        handleEvents();
        update(dt);
        render();
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            running = false;
        }
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
            running = false;
        }
    }
}

void Game::update(float dt) {
    player->update(dt);

    for (auto& e : enemies) {
        e->update(dt);
    }

    if (player->isAttacking()) {
        for (auto& e : enemies) {
            if (SDL_HasRectIntersectionFloat(&player->getAttackRect(), &e->getRect())) {
                // Push the enemy directly away from the player
                float dx = (e->getX() + e->getRect().w / 2.0f) - (player->getX() + player->getRect().w / 2.0f);
                float dy = (e->getY() + e->getRect().h / 2.0f) - (player->getY() + player->getRect().h / 2.0f);
                float len = std::sqrt(dx * dx + dy * dy);
                if (len > 0.0f) {
                    dx /= len;
                    dy /= len;
                }
                else {
                    dx = 1.0f;   // exactly on top of the player: push right
                    dy = 0.0f;
                }
                e->takeDamage(1, dx, dy);
            }
        }
    }

    // Remove dead enemies
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [](const std::unique_ptr<Enemy>& e) { return e->isDead(); }),
        enemies.end());
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
    SDL_RenderClear(renderer);

    player->render(renderer);

    for (auto& e : enemies) {
        e->render(renderer);
    }

    SDL_RenderPresent(renderer);
}

void Game::shutdown() {
    enemies.clear();      // enemies first, they point at the player
    player.reset();
    if (renderer) { SDL_DestroyRenderer(renderer); renderer = nullptr; }
    if (window) { SDL_DestroyWindow(window);     window = nullptr; }
    SDL_Quit();
}