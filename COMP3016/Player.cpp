#include "Player.h"
#include <cmath>

Player::Player(float x, float y)
    : Entity(x, y, 32.0f, 32.0f) {
}

void Player::update(float dt) {
    dodgeCooldown.update(dt);
    attackCooldown.update(dt);

    // End the attack hitbox when its time is up
    if (attacking) {
        attackTimer -= dt;
        if (attackTimer <= 0.0f) attacking = false;
    }

    // Mid-roll: keep moving in the roll direction, ignore normal input
    if (dodging) {
        rect.x += rollDirX * dodgeSpeed * dt;
        rect.y += rollDirY * dodgeSpeed * dt;
        dodgeTimer -= dt;
        if (dodgeTimer <= 0.0f) dodging = false;
        return;
    }

    const bool* keys = SDL_GetKeyboardState(nullptr);

    float dx = (float)keys[SDL_SCANCODE_D] - (float)keys[SDL_SCANCODE_A];
    float dy = (float)keys[SDL_SCANCODE_S] - (float)keys[SDL_SCANCODE_W];

    float len = std::sqrt(dx * dx + dy * dy);
    if (len > 0.0f) {
        dx /= len;
        dy /= len;
        lastDirX = dx;
        lastDirY = dy;
    }

    // Start a roll
    if (keys[SDL_SCANCODE_SPACE] && dodgeCooldown.tryUse()) {
        dodging = true;
        dodgeTimer = dodgeDuration;
        rollDirX = lastDirX;
        rollDirY = lastDirY;
        return;
    }

    // Start an attack toward the mouse
    float mx, my;
    SDL_MouseButtonFlags buttons = SDL_GetMouseState(&mx, &my);
    if ((buttons & SDL_BUTTON_LMASK) && attackCooldown.tryUse()) {
        float cx = rect.x + rect.w / 2.0f;
        float cy = rect.y + rect.h / 2.0f;
        float ax = mx - cx;
        float ay = my - cy;
        float alen = std::sqrt(ax * ax + ay * ay);
        if (alen > 0.0f) {
            ax /= alen;
            ay /= alen;
        }
        else {
            ax = lastDirX;
            ay = lastDirY;
        }

        // Place the hitbox in front of the player, centred 40px out
        attackRect.x = cx + ax * 40.0f - attackRect.w / 2.0f;
        attackRect.y = cy + ay * 40.0f - attackRect.h / 2.0f;
        attacking = true;
        attackTimer = attackDuration;
    }

    rect.x += dx * speed * dt;
    rect.y += dy * speed * dt;
}

void Player::render(SDL_Renderer* renderer) {
    if (dodging) {
        SDL_SetRenderDrawColor(renderer, 120, 200, 255, 255);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 80, 200, 120, 255);
    }
    SDL_RenderFillRect(renderer, &rect);

    if (attacking) {
        SDL_SetRenderDrawColor(renderer, 255, 160, 40, 255);   // orange swing
        SDL_RenderFillRect(renderer, &attackRect);
    }
}