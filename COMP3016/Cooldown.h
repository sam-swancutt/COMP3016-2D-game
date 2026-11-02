#pragma once

class Cooldown {
public:
    explicit Cooldown(float duration = 1.0f) : duration(duration) {}

    void update(float dt) {
        if (timer > 0.0f) timer -= dt;
    }

    bool ready() const { return timer <= 0.0f; }

    // Returns true and starts the cooldown if it was ready
    bool tryUse() {
        if (!ready()) return false;
        timer = duration;
        return true;
    }

private:
    float duration;
    float timer = 0.0f;
};