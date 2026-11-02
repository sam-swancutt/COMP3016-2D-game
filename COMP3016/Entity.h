#pragma once
#include <SDL3/SDL.h>

class Entity {
public:
    Entity(float x, float y, float w, float h);
    virtual ~Entity() = default;

    virtual void update(float dt) = 0;
    virtual void render(SDL_Renderer* renderer) = 0;

    float getX() const { return rect.x; }
    float getY() const { return rect.y; }
    const SDL_FRect& getRect() const { return rect; }

protected:
    SDL_FRect rect;   // position and size
};