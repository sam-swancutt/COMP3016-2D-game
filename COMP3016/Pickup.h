#pragma once
#include "Entity.h"
#include "Item.h"

// An item lying on the floor
class Pickup : public Entity {
public:
    Pickup(float centreX, float centreY, const Item& item, float pickupDelay = 0.0f);

    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;

    const Item& getItem() const { return item; }
    bool canPickUp() const { return delay <= 0.0f; }

private:
    Item item;
    float delay;      // seconds before it can be collected
};