#include "Player.h"
#include "Assets.h"
#include <cmath>

Player::Player(float x, float y)
    : Entity(x, y, 32.0f, 32.0f) {
}

void Player::takeDamage(int amount) {
    if (dodging) return;                  // invincible during a roll
    if (hurtCooldown.tryUse()) {
        health.takeDamage(amount);
    }
}

void Player::update(float dt) {
    dodgeCooldown.update(dt);
    attackCooldown.update(dt);
    hurtCooldown.update(dt);

    if (attacking) {
        attackTimer -= dt;
        if (attackTimer <= 0.0f) attacking = false;
    }

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

    if (keys[SDL_SCANCODE_SPACE] && dodgeCooldown.tryUse()) {
        dodging = true;
        dodgeTimer = dodgeDuration;
        rollDirX = lastDirX;
        rollDirY = lastDirY;
        return;
    }

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

        attackRect.x = cx + ax * 40.0f - attackRect.w / 2.0f;
        attackRect.y = cy + ay * 40.0f - attackRect.h / 2.0f;
        attacking = true;
        attackTimer = attackDuration;
    }

    rect.x += dx * speed * dt;
    rect.y += dy * speed * dt;
}

void Player::render(SDL_Renderer* renderer) {
    if (Assets::player) {
        // Tint the sprite: light blue while rolling, pink when just hit
        Uint8 r = 255, g = 255, b = 255;
        if (dodging) { r = 120; g = 200; b = 255; }
        else if (!hurtCooldown.ready()) { r = 255; g = 120; b = 120; }
        SDL_SetTextureColorMod(Assets::player, r, g, b);

        SDL_FlipMode flip = (lastDirX < 0.0f) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        SDL_RenderTextureRotated(renderer, Assets::player, nullptr, &rect, 0.0, nullptr, flip);
    }
    else {
        // No sprite loaded: coloured square
        if (dodging) {
            SDL_SetRenderDrawColor(renderer, 120, 200, 255, 255);
        }
        else if (!hurtCooldown.ready()) {
            SDL_SetRenderDrawColor(renderer, 255, 120, 120, 255);
        }
        else {
            SDL_SetRenderDrawColor(renderer, 80, 200, 120, 255);
        }
        SDL_RenderFillRect(renderer, &rect);
    }

    if (attacking) {
        SDL_SetRenderDrawColor(renderer, 255, 160, 40, 255);
        SDL_RenderFillRect(renderer, &attackRect);
    }
}

bool Player::pickUp(const Item& item) {
    return inventory.add(item);
}

void Player::useItem(int slot) {
    if (slot < 0 || slot >= Inventory::Size) return;
    Item chosen = inventory.get(slot);    // copy before touching the slot
    if (chosen.isEmpty()) return;

    if (chosen.getType() == ItemType::Potion) {
        if (health.isFull()) return;      // don't waste it at full health
        health.heal(chosen.getPower());
        inventory.take(slot);
    }
    else {
        inventory.set(slot, weapon);      // old weapon (or empty) goes back in the slot
        weapon = chosen;
    }
}

int Player::getDamage() const {
    return 1 + (weapon.isEmpty() ? 0 : weapon.getPower());
}