#pragma once
#include "Entity.h"
#include "Item.h"

// An item lying on the floor
class Pickup : public Entity {
public:
    Pickup(float centreX, float centreY, const Item& item);

    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;

    const Item& getItem() const { return item; }

private:
    Item item;
};