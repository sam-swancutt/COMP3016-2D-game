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
        finished = true;               // boss room will start here later
        return;
    }

    level++;
    spawnLevel(enemies, player);
    delayTimer = 1.5f;                 // pause after the next clear
}

void LevelManager::spawnLevel(std::vector<std::unique_ptr<Enemy>>& enemies, const Entity* player) {
    static std::mt19937 rng{ std::random_device{}() };

    int count = 2 + level;             // level 1 = 3 enemies, level 2 = 4, ...
    const float size = 28.0f;

    for (int i = 0; i < count; i++) {
        float x = 0.0f, y = 0.0f;

        // Pick a random spot on the edge of the screen, away from the player
        for (int tries = 0; tries < 20; tries++) {
            int edge = std::uniform_int_distribution<int>(0, 3)(rng);
            float along = std::uniform_real_distribution<float>(0.0f, 1.0f)(rng);

            switch (edge) {
            case 0: x = along * (screenW - size); y = 0.0f; break;                  // top
            case 1: x = along * (screenW - size); y = screenH - size; break;        // bottom
            case 2: x = 0.0f; y = along * (screenH - size); break;                  // left
            default: x = screenW - size; y = along * (screenH - size); break;       // right
            }

            float dx = x - player->getX();
            float dy = y - player->getY();
            if (std::sqrt(dx * dx + dy * dy) > 250.0f) break;
        }

        enemies.push_back(std::make_unique<Enemy>(x, y, player));
    }
}