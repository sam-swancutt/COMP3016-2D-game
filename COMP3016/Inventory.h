#pragma once
#include <array>
#include "Item.h"

class Inventory {
public:
    static constexpr int Size = 4;

    // Returns false if every slot is full
    bool add(const Item& item) {
        for (auto& s : slots) {
            if (s.isEmpty()) { s = item; return true; }
        }
        return false;
    }

    const Item& get(int index) const { return slots[index]; }

    void set(int index, const Item& value) {
        if (index >= 0 && index < Size) slots[index] = value;
    }

    void take(int index) {
        if (index >= 0 && index < Size) slots[index] = Item();   // back to empty
    }

private:
    std::array<Item, Size> slots;
};