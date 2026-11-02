#pragma once
#include "Entity.h"
#include "Cooldown.h"
#include "Health.h"

class Player : public Entity {
public:
    Player(float x, float y);

    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;

    void takeDamage(int amount);
    bool isDead() const { return health.isDead(); }
    float healthFraction() const { return health.fraction(); }

    bool isInvincible() const { return dodging; }
    bool isAttacking() const { return attacking; }
    const SDL_FRect& getAttackRect() const { return attackRect; }

private:
    float speed = 250.0f;

    Health health{ 5 };
    Cooldown hurtCooldown{ 1.0f };    // brief invulnerability after being hit

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
    float attackDuration = 0.12f;
    SDL_FRect attackRect{ 0.0f, 0.0f, 48.0f, 48.0f };
};