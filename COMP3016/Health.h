#pragma once
#include <algorithm>

class Health {
public:
    explicit Health(int maxHealth = 3) : maxHp(maxHealth), hp(maxHealth) {}

    void takeDamage(int amount) { hp = std::max(0, hp - amount); }
    void heal(int amount) { hp = std::min(maxHp, hp + amount); }
    bool isDead() const { return hp <= 0; }
    bool isFull() const { return hp >= maxHp; }
    float fraction() const { return (float)hp / (float)maxHp; }

private:
    int maxHp;
    int hp;
};