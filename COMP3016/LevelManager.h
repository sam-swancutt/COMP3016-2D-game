#pragma once
#include <vector>
#include <memory>
#include "Enemy.h"

class LevelManager {
public:
    LevelManager(int screenW, int screenH, int totalLevels = 3);

    void reset();
    void update(float dt, std::vector<std::unique_ptr<Enemy>>& enemies, const Entity* player);

    int getLevel() const { return level; }
    int getTotalLevels() const { return totalLevels; }
    bool isFinished() const { return finished; }   // all rooms cleared

private:
    void spawnLevel(std::vector<std::unique_ptr<Enemy>>& enemies, const Entity* player);

    int screenW;
    int screenH;
    int totalLevels;
    int level = 0;            // 0 = nothing spawned yet
    bool finished = false;
    float delayTimer = 1.0f;  // pause before the next wave
};