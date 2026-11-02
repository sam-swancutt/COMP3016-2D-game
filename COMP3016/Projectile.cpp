#include "Projectile.h"

Projectile::Projectile(float x, float y, float dirX, float dirY, float speed)
    : Entity(x - 6.0f, y - 6.0f, 12.0f, 12.0f),
    velX(dirX* speed), velY(dirY* speed) {
}

void Projectile::update(float dt) {
    rect.x += velX * dt;
    rect.y += velY * dt;
    lifetime -= dt;
}

void Projectile::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 90, 200, 255);
    SDL_RenderFillRect(renderer, &rect);
}