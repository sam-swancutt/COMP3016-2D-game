#pragma once
#include <vector>
#include <memory>
#include "Enemy.h"

class LevelManager {
public:
    LevelManager(int screenW, int screenH, int totalLevels = 3);

    void reset();
    void update(float dt, std::vector<std::unique_ptr<Enemy>>& enemies, const Entity* player);

    void setExtraEnemies(int n) { extraEnemies = n; }

    int getLevel() const { return level; }
    int getTotalLevels() const { return totalLevels; }
    bool isFinished() const { return finished; }

private:
    void spawnLevel(std::vector<std::unique_ptr<Enemy>>& enemies, const Entity* player);

    int screenW;
    int screenH;
    int totalLevels;
    int level = 0;
    int extraEnemies = 0;
    bool finished = false;
    float delayTimer = 1.0f;
};