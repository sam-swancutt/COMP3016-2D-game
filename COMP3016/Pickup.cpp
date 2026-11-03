#include "Pickup.h"

Pickup::Pickup(float centreX, float centreY, const Item& item)
    : Entity(centreX - 12.0f, centreY - 12.0f, 24.0f, 24.0f), item(item) {
}

void Pickup::update(float dt) {}

void Pickup::render(SDL_Renderer* renderer) {
    item.render(renderer, rect);
}