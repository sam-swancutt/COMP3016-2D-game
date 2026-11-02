#pragma once
#include "Entity.h"
#include "Health.h"
#include "Cooldown.h"

class Enemy : public Entity {
public:
    Enemy(float x, float y, const Entity* target);

    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;

    // dirX/dirY is the direction to push the enemy (should be normalised)
    void takeDamage(int amount, float dirX, float dirY);
    bool isDead() const { return health.isDead(); }

private:
    const Entity* target;
    float speed = 100.0f;
    Health health{ 3 };
    Cooldown hurtCooldown{ 0.3f };

    // Knockback
    float knockVelX = 0.0f;
    float knockVelY = 0.0f;
    float knockStrength = 450.0f;   // starting push speed
    float knockFriction = 8.0f;     // higher = stops sooner
};