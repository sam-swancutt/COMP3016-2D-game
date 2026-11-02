#pragma once
#include <vector>
#include <memory>
#include "Entity.h"
#include "Health.h"
#include "Cooldown.h"
#include "Projectile.h"

class Boss : public Entity {
public:
    Boss(float x, float y, const Entity* target);

    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;

    void takeDamage(int amount);
    bool isDead() const { return health.isDead(); }
    float healthFraction() const { return health.fraction(); }
    int getPhase() const;     // 1, 2 or 3, based on remaining health

    std::vector<std::unique_ptr<Projectile>>& getProjectiles() { return projectiles; }

private:
    enum class State { Chase, Telegraph, Charge, Recover };

    void fireRing(int count, float speed);

    const Entity* target;
    Health health{ 20 };
    Cooldown hurtCooldown{ 0.3f };

    State state = State::Chase;
    float stateTimer = 2.0f;
    bool chargeNext = true;          // phase 3 alternates charge and ring
    float chargeDirX = 0.0f;
    float chargeDirY = 0.0f;
    float ringOffset = 0.0f;         // rotates each ring so the gaps move

    std::vector<std::unique_ptr<Projectile>> projectiles;

    static constexpr float arenaW = 1280.0f;
    static constexpr float arenaH = 720.0f;
};