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

    startRun();
    return true;
}

void Game::startRun() {
    boss.reset();         // boss and enemies point at the player, so they go first
    enemies.clear();
    player = std::make_unique<Player>(640.0f, 360.0f);
    bossSpawned = false;
    won = false;
    levelManager.reset();
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
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_RETURN && won) {
            startRun();   // play again after winning
        }
    }
}

void Game::update(float dt) {
    // Victory screen: player can still move, nothing else happens
    if (won) {
        player->update(dt);
        return;
    }

    player->update(dt);

    for (auto& e : enemies) {
        e->update(dt);
    }

    // Player attack hits enemies
    if (player->isAttacking()) {
        for (auto& e : enemies) {
            if (SDL_HasRectIntersectionFloat(&player->getAttackRect(), &e->getRect())) {
                float dx = (e->getX() + e->getRect().w / 2.0f) - (player->getX() + player->getRect().w / 2.0f);
                float dy = (e->getY() + e->getRect().h / 2.0f) - (player->getY() + player->getRect().h / 2.0f);
                float len = std::sqrt(dx * dx + dy * dy);
                if (len > 0.0f) {
                    dx /= len;
                    dy /= len;
                }
                else {
                    dx = 1.0f;
                    dy = 0.0f;
                }
                e->takeDamage(1, dx, dy);
            }
        }
    }

    // Enemies hurt the player on contact
    for (auto& e : enemies) {
        if (SDL_HasRectIntersectionFloat(&player->getRect(), &e->getRect())) {
            player->takeDamage(1);
        }
    }

    // Remove dead enemies
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [](const std::unique_ptr<Enemy>& e) { return e->isDead(); }),
        enemies.end());

    // Spawn the next wave when the room is clear
    levelManager.update(dt, enemies, player.get());

    // All rooms cleared: spawn the boss once
    if (levelManager.isFinished() && !bossSpawned) {
        boss = std::make_unique<Boss>(592.0f, 60.0f, player.get());
        bossSpawned = true;
    }

    if (boss) {
        boss->update(dt);

        // Player attack hits the boss
        if (player->isAttacking() &&
            SDL_HasRectIntersectionFloat(&player->getAttackRect(), &boss->getRect())) {
            boss->takeDamage(1);
        }

        // Touching the boss hurts
        if (SDL_HasRectIntersectionFloat(&player->getRect(), &boss->getRect())) {
            player->takeDamage(1);
        }

        // Projectiles hurt, unless the player is mid-roll (they pass through)
        for (auto& p : boss->getProjectiles()) {
            if (SDL_HasRectIntersectionFloat(&player->getRect(), &p->getRect())) {
                player->takeDamage(1);
                if (!player->isInvincible()) p->expire();
            }
        }

        // Boss dead: you win
        if (boss->isDead()) {
            boss.reset();
            won = true;
        }
    }

    // Player died: restart the run
    if (player->isDead()) {
        startRun();
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
    SDL_RenderClear(renderer);

    player->render(renderer);

    for (auto& e : enemies) {
        e->render(renderer);
    }

    if (boss) {
        boss->render(renderer);

        // Boss health bar, top centre
        float bf = boss->healthFraction();
        SDL_FRect bbg{ 340.0f, 20.0f, 600.0f, 20.0f };
        SDL_FRect bbar{ 340.0f, 20.0f, 600.0f * bf, 20.0f };
        SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
        SDL_RenderFillRect(renderer, &bbg);
        SDL_SetRenderDrawColor(renderer, (Uint8)(255 * (1.0f - bf)), (Uint8)(255 * bf), 0, 255);
        SDL_RenderFillRect(renderer, &bbar);
    }

    // HUD: player health bar
    float f = player->healthFraction();
    SDL_FRect bg{ 20.0f, 20.0f, 200.0f, 16.0f };
    SDL_FRect bar{ 20.0f, 20.0f, 200.0f * f, 16.0f };
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderFillRect(renderer, &bg);
    SDL_SetRenderDrawColor(renderer, (Uint8)(255 * (1.0f - f)), (Uint8)(255 * f), 0, 255);
    SDL_RenderFillRect(renderer, &bar);

    // HUD: one square per room, gold when all are cleared
    for (int i = 0; i < levelManager.getTotalLevels(); i++) {
        SDL_FRect pip{ 20.0f + i * 24.0f, 46.0f, 16.0f, 16.0f };
        if (levelManager.isFinished()) {
            SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
        }
        else if (i < levelManager.getLevel()) {
            SDL_SetRenderDrawColor(renderer, 255, 160, 40, 255);
        }
        else {
            SDL_SetRenderDrawColor(renderer, 60, 60, 70, 255);
        }
        SDL_RenderFillRect(renderer, &pip);
    }

    // Victory text (SDL's built-in debug font, no extra library needed)
    if (won) {
        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
        SDL_SetRenderScale(renderer, 3.0f, 3.0f);
        SDL_RenderDebugText(renderer, 81.0f, 113.0f, "YOU WIN");
        SDL_SetRenderScale(renderer, 1.0f, 1.0f);
        SDL_RenderDebugText(renderer, 540.0f, 400.0f, "Press Enter to play again");
    }

    SDL_RenderPresent(renderer);
}

void Game::shutdown() {
    boss.reset();
    enemies.clear();
    player.reset();
    if (renderer) { SDL_DestroyRenderer(renderer); renderer = nullptr; }
    if (window) { SDL_DestroyWindow(window);     window = nullptr; }
    SDL_Quit();
}