#pragma once
#include "Entity.h"

class Player : public Entity {
public:
    Player(float x, float y);

    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;

private:
    float speed = 250.0f;
};