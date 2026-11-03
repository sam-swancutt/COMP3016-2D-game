#include "Enemy.h"
#include "Assets.h"
#include <cmath>
#include <algorithm>

Enemy::Enemy(float x, float y, const Entity* target)
    : Entity(x, y, 28.0f, 28.0f), target(target) {
}

void Enemy::update(float dt) {
    hurtCooldown.update(dt);

    // Apply knockback, then slow it down
    rect.x += knockVelX * dt;
    rect.y += knockVelY * dt;
    float decay = std::max(0.0f, 1.0f - knockFriction * dt);
    knockVelX *= decay;
    knockVelY *= decay;

    // Don't chase while being knocked back
    float knockSpeed = std::sqrt(knockVelX * knockVelX + knockVelY * knockVelY);
    if (knockSpeed > 40.0f) return;

    // Move toward the centre of the target
    float dx = (target->getX() + target->getRect().w / 2.0f) - (rect.x + rect.w / 2.0f);
    float dy = (target->getY() + target->getRect().h / 2.0f) - (rect.y + rect.h / 2.0f);
    float len = std::sqrt(dx * dx + dy * dy);
    if (len > 1.0f) {
        rect.x += (dx / len) * speed * dt;
        rect.y += (dy / len) * speed * dt;
    }
}

void Enemy::takeDamage(int amount, float dirX, float dirY) {
    if (hurtCooldown.tryUse()) {
        health.takeDamage(amount);
        knockVelX = dirX * knockStrength;
        knockVelY = dirY * knockStrength;
    }
}

void Enemy::render(SDL_Renderer* renderer) {
    bool hit = !hurtCooldown.ready();

    if (Assets::enemy) {
        // Face the player
        SDL_FlipMode flip = (target->getX() < rect.x) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        SDL_SetTextureColorMod(Assets::enemy, 255, 255, 255);
        SDL_RenderTextureRotated(renderer, Assets::enemy, nullptr, &rect, 0.0, nullptr, flip);

        // White flash over the sprite when hit
        if (hit) {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 170);
            SDL_RenderFillRect(renderer, &rect);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }
    }
    else {
        if (hit) SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        else     SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);
        SDL_RenderFillRect(renderer, &rect);
    }

    // Health bar above the enemy: green when full, red when low
    float f = health.fraction();
    SDL_FRect bg{ rect.x, rect.y - 8.0f, rect.w, 4.0f };
    SDL_FRect bar{ rect.x, rect.y - 8.0f, rect.w * f, 4.0f };
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderFillRect(renderer, &bg);
    SDL_SetRenderDrawColor(renderer, (Uint8)(255 * (1.0f - f)), (Uint8)(255 * f), 0, 255);
    SDL_RenderFillRect(renderer, &bar);
}