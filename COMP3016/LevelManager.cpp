#include "LevelManager.h"
#include <random>
#include <cmath>

LevelManager::LevelManager(int screenW, int screenH, int totalLevels)
    : screenW(screenW), screenH(screenH), totalLevels(totalLevels) {
}

void LevelManager::reset() {
    level = 0;
    finished = false;
    delayTimer = 1.0f;
}

void LevelManager::update(float dt, std::vector<std::unique_ptr<Enemy>>& enemies, const Entity* player) {
    if (finished) return;
    if (!enemies.empty()) return;      // room not cleared yet

    delayTimer -= dt;
    if (delayTimer > 0.0f) return;

    if (level >= totalLevels) {
        finished = true;               // boss spawns from Game
        return;
    }

    level++;
    spawnLevel(enemies, player);
    delayTimer = 1.5f;
}

void LevelManager::spawnLevel(std::vector<std::unique_ptr<Enemy>>& enemies, const Entity* player) {
    static std::mt19937 rng{ std::random_device{}() };

    int count = 2 + level + extraEnemies;   // base 3, 4, 5 plus difficulty
    SDL_Log("Spawning %d enemies for room %d", count, level);
    const float size = 28.0f;

    for (int i = 0; i < count; i++) {
        float x = 0.0f, y = 0.0f;

        for (int tries = 0; tries < 20; tries++) {
            int edge = std::uniform_int_distribution<int>(0, 3)(rng);
            float along = std::uniform_real_distribution<float>(0.0f, 1.0f)(rng);

            switch (edge) {
            case 0: x = along * (screenW - size); y = 0.0f; break;
            case 1: x = along * (screenW - size); y = screenH - size; break;
            case 2: x = 0.0f; y = along * (screenH - size); break;
            default: x = screenW - size; y = along * (screenH - size); break;
            }

            float dx = x - player->getX();
            float dy = y - player->getY();
            if (std::sqrt(dx * dx + dy * dy) > 250.0f) break;
        }

        enemies.push_back(std::make_unique<Enemy>(x, y, player));
    }
}