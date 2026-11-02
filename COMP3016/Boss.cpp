#include "Boss.h"
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

    if (!hurtCooldown.ready()) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);        // white: just hit
    }
    else {
        switch (state) {
        case State::Chase:
            SDL_SetRenderDrawColor(renderer, 140, 60, 180, 255);     // purple
            break;
        case State::Telegraph:
            if ((int)(stateTimer * 10.0f) % 2 == 0)
                SDL_SetRenderDrawColor(renderer, 255, 210, 0, 255);  // flashing yellow: attack coming
            else
                SDL_SetRenderDrawColor(renderer, 140, 60, 180, 255);
            break;
        case State::Charge:
            SDL_SetRenderDrawColor(renderer, 255, 60, 60, 255);      // red: charging
            break;
        case State::Recover:
            SDL_SetRenderDrawColor(renderer, 100, 120, 180, 255);    // blue-grey: stunned
            break;
        }
    }
    SDL_RenderFillRect(renderer, &rect);
}