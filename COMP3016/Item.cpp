#include "Item.h"
#include "Assets.h"
#include <random>

namespace {
    // Indexed by rarity: Common, Rare, Legendary
    const int potionPower[3] = { 1, 2, 99 };   // 99 = full heal
    const int weaponPower[3] = { 1, 2, 3 };    // extra damage per hit
}

Item::Item(ItemType type, Rarity rarity)
    : type(type), rarity(rarity), empty(false) {
}

int Item::getPower() const {
    int r = (int)rarity;
    return (type == ItemType::Potion) ? potionPower[r] : weaponPower[r];
}

std::string Item::getName() const {
    const char* r = (rarity == Rarity::Common) ? "Common" :
        (rarity == Rarity::Rare) ? "Rare" : "Legendary";
    const char* t = (type == ItemType::Potion) ? "Potion" : "Sword";
    return std::string(r) + " " + t;
}

SDL_Color Item::getRarityColour() const {
    switch (rarity) {
    case Rarity::Common: return SDL_Color{ 200, 200, 205, 255 };   // white
    case Rarity::Rare:   return SDL_Color{ 70, 140, 255, 255 };    // blue
    default:             return SDL_Color{ 255, 200, 40, 255 };    // gold
    }
}

void Item::render(SDL_Renderer* renderer, const SDL_FRect& area) const {
    // Border in the rarity colour, thicker for rarer items.
    // The thickness scales with the size, so small floor drops still look right.
    SDL_Color c = getRarityColour();
    float base = (rarity == Rarity::Common) ? 2.0f : (rarity == Rarity::Rare) ? 3.0f : 5.0f;
    float t = base * (area.w / 48.0f);
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, 255);
    SDL_RenderFillRect(renderer, &area);

    SDL_FRect inner{ area.x + t, area.y + t, area.w - 2.0f * t, area.h - 2.0f * t };
    SDL_Texture* icon = (type == ItemType::Potion) ? Assets::potion : Assets::sword;

    if (icon) {
        // Dark panel with the sprite on top
        SDL_SetRenderDrawColor(renderer, 30, 30, 42, 255);
        SDL_RenderFillRect(renderer, &inner);
        SDL_RenderTexture(renderer, icon, nullptr, &inner);
    }
    else {
        // No sprite loaded: plain colours (red = potion, steel grey = weapon)
        if (type == ItemType::Potion) SDL_SetRenderDrawColor(renderer, 220, 70, 90, 255);
        else                          SDL_SetRenderDrawColor(renderer, 150, 155, 175, 255);
        SDL_RenderFillRect(renderer, &inner);
    }
}

bool Item::rollDrop(Item& out) {
    static std::mt19937 rng{ std::random_device{}() };
    std::uniform_real_distribution<float> d(0.0f, 1.0f);

    if (d(rng) > 0.35f) return false;          // 35% of enemies drop something

    float r = d(rng);
    Rarity rarity = (r < 0.70f) ? Rarity::Common :
        (r < 0.95f) ? Rarity::Rare : Rarity::Legendary;
    ItemType type = (d(rng) < 0.5f) ? ItemType::Potion : ItemType::Weapon;
    out = Item(type, rarity);
    return true;
}