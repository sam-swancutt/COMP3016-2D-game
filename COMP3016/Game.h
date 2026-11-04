#pragma once
#include <SDL3/SDL.h>
#include <memory>
#include <vector>
#include "Player.h"
#include "Enemy.h"
#include "Boss.h"
#include "Pickup.h"
#include "LevelManager.h"

class Game {
public:
    Game() = default;
    ~Game();

    bool init(const char* title, int width, int height);
    void run();

private:
    enum class GameState { Menu, Playing };

    void handleEvents();
    void update(float dt);
    void render();
    void renderMenu();
    void renderInventory();
    void shutdown();
    void startRun();
    void startGame();
    SDL_FRect buttonRect(int index) const;
    SDL_FRect slotRect(int index) const;     // inventory slot positions
    SDL_FRect dropButtonRect() const;        // the DROP button
    void dropFromSlot(int slot);
    void drawText(const char* text, float centerX, float y, float scale);

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    bool running = false;

    GameState state = GameState::Menu;
    int selectedDifficulty = 1;
    bool dropMode = false;                   // true after clicking DROP (or pressing Q)

    std::unique_ptr<Player> player;
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<std::unique_ptr<Pickup>> pickups;
    std::unique_ptr<Boss> boss;
    bool bossSpawned = false;
    bool won = false;
    LevelManager levelManager{ 1280, 720, 3 };
};