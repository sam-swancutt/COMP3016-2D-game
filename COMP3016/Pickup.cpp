#include "Pickup.h"

Pickup::Pickup(float centreX, float centreY, const Item& item, float pickupDelay)
    : Entity(centreX - 12.0f, centreY - 12.0f, 24.0f, 24.0f), item(item), delay(pickupDelay) {
}

void Pickup::update(float dt) {
    if (delay > 0.0f) delay -= dt;
}

void Pickup::render(SDL_Renderer* renderer) {
    // Flash while it can't be picked up yet
    if (!canPickUp() && ((int)(delay * 8.0f) % 2 == 0)) return;
    item.render(renderer, rect);
}