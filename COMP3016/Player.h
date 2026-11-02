#pragma once
#include "Entity.h"
#include "Cooldown.h"

class Player : public Entity {
public:
    Player(float x, float y);

    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;

    bool isInvincible() const { return dodging; }
    bool isAttacking() const { return attacking; }
    const SDL_FRect& getAttackRect() const { return attackRect; }

private:
    float speed = 250.0f;

    // Dodge roll
    Cooldown dodgeCooldown{ 1.0f };
    bool dodging = false;
    float dodgeTimer = 0.0f;
    float dodgeDuration = 0.2f;
    float dodgeSpeed = 700.0f;
    float rollDirX = 0.0f;
    float rollDirY = 0.0f;
    float lastDirX = 0.0f;
    float lastDirY = 1.0f;

    // Attack
    Cooldown attackCooldown{ 0.4f };
    bool attacking = false;
    float attackTimer = 0.0f;
    float attackDuration = 0.12f;     // how long the hitbox stays active
    SDL_FRect attackRect{ 0.0f, 0.0f, 48.0f, 48.0f };
};