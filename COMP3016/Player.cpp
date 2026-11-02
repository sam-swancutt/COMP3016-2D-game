#include "Player.h"
#include <cmath>

Player::Player(float x, float y)
    : Entity(x, y, 32.0f, 32.0f) {
}

void Player::update(float dt) {
    const bool* keys = SDL_GetKeyboardState(nullptr);

    float dx = (float)keys[SDL_SCANCODE_D] - (float)keys[SDL_SCANCODE_A];
    float dy = (float)keys[SDL_SCANCODE_S] - (float)keys[SDL_SCANCODE_W];

    // normalise so diagonals aren't faster
    float len = std::sqrt(dx * dx + dy * dy);
    if (len > 0.0f) {
        dx /= len;
        dy /= len;
    }

    rect.x += dx * speed * dt;
    rect.y += dy * speed * dt;
}

void Player::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 80, 200, 120, 255);
    SDL_RenderFillRect(renderer, &rect);
}