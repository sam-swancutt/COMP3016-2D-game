#pragma once
#include <SDL3/SDL.h>
#include <string>

enum class Rarity { Common, Rare, Legendary };
enum class ItemType { Potion, Weapon };

class Item {
public:
    Item() = default;                      // an empty item (used for empty slots)
    Item(ItemType type, Rarity rarity);

    bool isEmpty() const { return empty; }
    ItemType getType() const { return type; }
    Rarity getRarity() const { return rarity; }
    int getPower() const;                  // heal amount (potion) or bonus damage (weapon)
    std::string getName() const;
    SDL_Color getRarityColour() const;

    void render(SDL_Renderer* renderer, const SDL_FRect& area) const;

    // Rolls for a random drop. Returns true and fills 'out' if something dropped.
    static bool rollDrop(Item& out);

private:
    ItemType type = ItemType::Potion;
    Rarity rarity = Rarity::Common;
    bool empty = true;
};