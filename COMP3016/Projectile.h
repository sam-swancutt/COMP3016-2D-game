#pragma once
#include "Entity.h"

class Projectile : public Entity {
public:
    Projectile(float x, float y, float dirX, float dirY, float speed);

    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;

    bool isExpired() const { return lifetime <= 0.0f; }
    void expire() { lifetime = 0.0f; }

private:
    float velX;
    float velY;
    float lifetime = 4.0f;   // seconds before it disappears
};