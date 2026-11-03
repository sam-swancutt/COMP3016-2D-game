#include "Boss.h"
#include "Assets.h"
#include <cmath>
#include <algorithm>

Boss::Boss(float x, float y, const Entity* target)
    : Entity(x, y, 96.0f, 96.0f), target(target) {
}

int Boss::getPhase() const {
    float f = health.fraction();
    if (f > 0.66f) return 1;
    if (f > 0.33f) return 2;
    return 3;
}

void Boss::takeDamage(int amount) {
    if (hurtCooldown.tryUse()) {
        health.takeDamage(amount);
    }
}

void Boss::fireRing(int count, float speed) {
    float cx = rect.x + rect.w / 2.0f;
    float cy = rect.y + rect.h / 2.0f;
    for (int i = 0; i < count; i++) {
        float angle = ringOffset + i * (2.0f * 3.14159265f / count);
        projectiles.push_back(std::make_unique<Projectile>(cx, cy, std::cos(angle), std::sin(angle), speed));
    }
    ringOffset += 0.3f;
}

void Boss::update(float dt) {
    hurtCooldown.update(dt);
    stateTimer -= dt;

    int phase = getPhase();

    // Update and clean up projectiles
    for (auto& p : projectiles) {
        p->update(dt);
    }
    projectiles.erase(
        std::remove_if(projectiles.begin(), projectiles.end(),
            [](const std::unique_ptr<Projectile>& p) { return p->isExpired(); }),
        projectiles.end());

    // Vector from the boss to the player
    float dx = (target->getX() + target->getRect().w / 2.0f) - (rect.x + rect.w / 2.0f);
    float dy = (target->getY() + target->getRect().h / 2.0f) - (rect.y + rect.h / 2.0f);
    float len = std::sqrt(dx * dx + dy * dy);

    switch (state) {
    case State::Chase: {
        float speed = 50.0f + 20.0f * phase;          // 70, 90, 110
        if (len > 1.0f) {
            rect.x += (dx / len) * speed * dt;
            rect.y += (dy / len) * speed * dt;
        }
        if (stateTimer <= 0.0f) {
            state = State::Telegraph;
            stateTimer = (phase == 3) ? 0.5f : 0.7f;  // warning time
        }
        break;
    }
    case State::Telegraph:
        // Stands still and flashes so the player can react
        if (stateTimer <= 0.0f) {
            bool doCharge = (phase == 3 && chargeNext);
            if (phase == 3) chargeNext = !chargeNext;

            if (doCharge) {
                chargeDirX = (len > 0.0f) ? dx / len : 1.0f;
                chargeDirY = (len > 0.0f) ? dy / len : 0.0f;
                state = State::Charge;
                stateTimer = 0.5f;
            }
            else {
                int count = (phase == 1) ? 8 : (phase == 2) ? 12 : 16;
                float speed = 180.0f + 40.0f * phase;
                fireRing(count, speed);
                state = State::Chase;
                stateTimer = (phase == 1) ? 2.5f : (phase == 2) ? 2.0f : 1.6f;
            }
        }
        break;
    case State::Charge:
        rect.x += chargeDirX * 650.0f * dt;
        rect.y += chargeDirY * 650.0f * dt;
        if (stateTimer <= 0.0f) {
            state = State::Recover;
            stateTimer = 0.8f;                         // stunned: free hits
        }
        break;
    case State::Recover:
        if (stateTimer <= 0.0f) {
            state = State::Chase;
            stateTimer = 1.6f;
        }
        break;
    }

    // Keep the boss inside the window
    rect.x = std::max(0.0f, std::min(rect.x, arenaW - rect.w));
    rect.y = std::max(0.0f, std::min(rect.y, arenaH - rect.h));
}

void Boss::render(SDL_Renderer* renderer) {
    for (auto& p : projectiles) {
        p->render(renderer);
    }

    bool hit = !hurtCooldown.ready();
    bool flashOn = ((int)(stateTimer * 10.0f) % 2 == 0);   // used for the telegraph flash

    if (Assets::boss) {
        // Tint the sprite by state so the attack pattern is still readable
        Uint8 r = 255, g = 255, b = 255;
        switch (state) {
        case State::Chase:     break;
        case State::Telegraph: if (flashOn) { r = 255; g = 210; b = 0; } break;   // flashing yellow
        case State::Charge:    r = 255; g = 90;  b = 90;  break;                  // red
        case State::Recover:   r = 150; g = 170; b = 255; break;                  // blue: stunned
        }
        SDL_SetTextureColorMod(Assets::boss, r, g, b);

        SDL_FlipMode flip = (target->getX() < rect.x) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
        SDL_RenderTextureRotated(renderer, Assets::boss, nullptr, &rect, 0.0, nullptr, flip);

        if (hit) {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 170);
            SDL_RenderFillRect(renderer, &rect);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }
    }
    else {
        if (hit) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        }
        else {
            switch (state) {
            case State::Chase:
                SDL_SetRenderDrawColor(renderer, 140, 60, 180, 255);
                break;
            case State::Telegraph:
                if (flashOn) SDL_SetRenderDrawColor(renderer, 255, 210, 0, 255);
                else         SDL_SetRenderDrawColor(renderer, 140, 60, 180, 255);
                break;
            case State::Charge:
                SDL_SetRenderDrawColor(renderer, 255, 60, 60, 255);
                break;
            case State::Recover:
                SDL_SetRenderDrawColor(renderer, 100, 120, 180, 255);
                break;
            }
        }
        SDL_RenderFillRect(renderer, &rect);
    }
}