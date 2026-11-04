#include "Game.h"
#include "Assets.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <string>

namespace {
    const char* difficultyNames[3] = { "EASY (+0 bots)", "NORMAL (+2 bots)", "HARD (+4 bots)" };
    const int difficultyExtra[3] = { 0, 2, 4 };

    bool inside(const SDL_FRect& r, float x, float y) {
        return x >= r.x && x <= r.x + r.w && y >= r.y && y <= r.y + r.h;
    }
}

Game::~Game() {
    shutdown();
}

bool Game::init(const char* title, int width, int height) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow(title, width, height, 0);
    if (!window) {
        SDL_Log("Window failed: %s", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        SDL_Log("Renderer failed: %s", SDL_GetError());
        return false;
    }

    Assets::load(renderer);   // load sprites once, after the renderer exists

    startRun();
    return true;
}

void Game::startRun() {
    boss.reset();
    enemies.clear();
    pickups.clear();
    player = std::make_unique<Player>(640.0f, 360.0f);   // new player = empty inventory
    bossSpawned = false;
    won = false;
    dropMode = false;
    levelManager.reset();
}

void Game::startGame() {
    levelManager.setExtraEnemies(difficultyExtra[selectedDifficulty]);
    startRun();
    state = GameState::Playing;
}

SDL_FRect Game::buttonRect(int index) const {
    return SDL_FRect{ 440.0f, 280.0f + index * 90.0f, 400.0f, 64.0f };
}

SDL_FRect Game::slotRect(int index) const {
    return SDL_FRect{ 20.0f + index * 60.0f, 650.0f, 48.0f, 48.0f };
}

SDL_FRect Game::dropButtonRect() const {
    return SDL_FRect{ 350.0f, 650.0f, 110.0f, 48.0f };
}

void Game::dropFromSlot(int slot) {
    Item item;
    if (!player->dropItem(slot, item)) return;   // empty slot: nothing to drop

    // Land it a short way from the player, away from the screen edge
    float cx = player->getX() + player->getRect().w / 2.0f;
    float cy = player->getY() + player->getRect().h / 2.0f;
    float dropY = (cy + 50.0f < 640.0f) ? cy + 50.0f : cy - 50.0f;

    // 1.5 second delay so it isn't picked straight back up
    pickups.push_back(std::make_unique<Pickup>(cx, dropY, item, 1.5f));
    dropMode = false;
}

void Game::drawText(const char* text, float centerX, float y, float scale) {
    float w = (float)std::strlen(text) * 8.0f * scale;
    SDL_SetRenderScale(renderer, scale, scale);
    SDL_RenderDebugText(renderer, (centerX - w / 2.0f) / scale, y / scale, text);
    SDL_SetRenderScale(renderer, 1.0f, 1.0f);
}

void Game::run() {
    running = true;
    Uint64 last = SDL_GetTicks();

    while (running) {
        Uint64 now = SDL_GetTicks();
        float dt = (now - last) / 1000.0f;
        last = now;

        handleEvents();
        update(dt);
        render();
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            running = false;
        }

        if (state == GameState::Menu) {
            if (event.type == SDL_EVENT_KEY_DOWN) {
                SDL_Keycode k = event.key.key;
                if (k == SDLK_W || k == SDLK_UP) {
                    selectedDifficulty = (selectedDifficulty + 2) % 3;
                }
                else if (k == SDLK_S || k == SDLK_DOWN) {
                    selectedDifficulty = (selectedDifficulty + 1) % 3;
                }
                else if (k == SDLK_RETURN) {
                    startGame();
                }
                else if (k == SDLK_ESCAPE) {
                    running = false;
                }
            }

            if (event.type == SDL_EVENT_MOUSE_MOTION || event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                float mx = (event.type == SDL_EVENT_MOUSE_MOTION) ? event.motion.x : event.button.x;
                float my = (event.type == SDL_EVENT_MOUSE_MOTION) ? event.motion.y : event.button.y;
                for (int i = 0; i < 3; i++) {
                    SDL_FRect r = buttonRect(i);
                    if (inside(r, mx, my)) {
                        selectedDifficulty = i;
                        if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
                            startGame();
                        }
                    }
                }
            }
        }
        else {
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
                state = GameState::Menu;
            }
            if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_RETURN && won) {
                state = GameState::Menu;
            }

            if (event.type == SDL_EVENT_KEY_DOWN && !event.key.repeat && !won) {
                SDL_Keycode k = event.key.key;

                // Q toggles drop mode
                if (k == SDLK_Q) {
                    dropMode = !dropMode;
                }

                // Keys 1-4: drop the item if drop mode is on, otherwise use or equip it
                if (k >= SDLK_1 && k <= SDLK_4) {
                    int slot = (int)(k - SDLK_1);
                    if (dropMode) dropFromSlot(slot);
                    else          player->useItem(slot);
                }
            }

            // Mouse: the DROP button, clicking a slot in drop mode, right-click to drop
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && !won) {
                float mx = event.button.x;
                float my = event.button.y;

                if (event.button.button == SDL_BUTTON_LEFT && inside(dropButtonRect(), mx, my)) {
                    dropMode = !dropMode;
                }
                else {
                    for (int i = 0; i < Inventory::Size; i++) {
                        if (!inside(slotRect(i), mx, my)) continue;
                        if (event.button.button == SDL_BUTTON_RIGHT ||
                            (event.button.button == SDL_BUTTON_LEFT && dropMode)) {
                            dropFromSlot(i);
                        }
                    }
                }
            }
        }
    }
}

void Game::update(float dt) {
    if (state == GameState::Menu) return;

    if (won) {
        player->update(dt);
        return;
    }

    player->update(dt);

    for (auto& e : enemies) {
        e->update(dt);
    }

    for (auto& p : pickups) {
        p->update(dt);    // counts down the pickup delay on dropped items
    }

    // Player attack hits enemies (damage depends on the equipped weapon)
    if (player->isAttacking()) {
        for (auto& e : enemies) {
            if (SDL_HasRectIntersectionFloat(&player->getAttackRect(), &e->getRect())) {
                float dx = (e->getX() + e->getRect().w / 2.0f) - (player->getX() + player->getRect().w / 2.0f);
                float dy = (e->getY() + e->getRect().h / 2.0f) - (player->getY() + player->getRect().h / 2.0f);
                float len = std::sqrt(dx * dx + dy * dy);
                if (len > 0.0f) {
                    dx /= len;
                    dy /= len;
                }
                else {
                    dx = 1.0f;
                    dy = 0.0f;
                }
                e->takeDamage(player->getDamage(), dx, dy);
            }
        }
    }

    // Enemies hurt the player on contact
    for (auto& e : enemies) {
        if (SDL_HasRectIntersectionFloat(&player->getRect(), &e->getRect())) {
            player->takeDamage(1);
        }
    }

    // Dead enemies may drop loot, then they're removed
    for (auto& e : enemies) {
        if (e->isDead()) {
            Item drop;
            if (Item::rollDrop(drop)) {
                pickups.push_back(std::make_unique<Pickup>(
                    e->getX() + e->getRect().w / 2.0f,
                    e->getY() + e->getRect().h / 2.0f,
                    drop));
            }
        }
    }
    enemies.erase(
        std::remove_if(enemies.begin(), enemies.end(),
            [](const std::unique_ptr<Enemy>& e) { return e->isDead(); }),
        enemies.end());

    // Walk over a pickup to collect it (it stays on the floor if the inventory is full
    // or if it was only just dropped)
    for (auto it = pickups.begin(); it != pickups.end(); ) {
        if ((*it)->canPickUp() &&
            SDL_HasRectIntersectionFloat(&player->getRect(), &(*it)->getRect()) &&
            player->pickUp((*it)->getItem())) {
            it = pickups.erase(it);
        }
        else {
            ++it;
        }
    }

    levelManager.update(dt, enemies, player.get());

    if (levelManager.isFinished() && !bossSpawned) {
        boss = std::make_unique<Boss>(592.0f, 60.0f, player.get());
        bossSpawned = true;
    }

    if (boss) {
        boss->update(dt);

        if (player->isAttacking() &&
            SDL_HasRectIntersectionFloat(&player->getAttackRect(), &boss->getRect())) {
            boss->takeDamage(player->getDamage());
        }

        if (SDL_HasRectIntersectionFloat(&player->getRect(), &boss->getRect())) {
            player->takeDamage(1);
        }

        for (auto& p : boss->getProjectiles()) {
            if (SDL_HasRectIntersectionFloat(&player->getRect(), &p->getRect())) {
                player->takeDamage(1);
                if (!player->isInvincible()) p->expire();
            }
        }

        if (boss->isDead()) {
            boss.reset();
            won = true;
        }
    }

    if (player->isDead()) {
        startRun();
    }
}

void Game::renderMenu() {
    SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
    drawText("HUNTER", 640.0f, 120.0f, 6.0f);

    SDL_SetRenderDrawColor(renderer, 200, 200, 210, 255);
    drawText("Choose a difficulty", 640.0f, 220.0f, 2.0f);

    for (int i = 0; i < 3; i++) {
        SDL_FRect r = buttonRect(i);
        bool selected = (i == selectedDifficulty);

        if (selected) SDL_SetRenderDrawColor(renderer, 255, 160, 40, 255);
        else          SDL_SetRenderDrawColor(renderer, 50, 50, 70, 255);
        SDL_RenderFillRect(renderer, &r);

        if (selected) SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
        else          SDL_SetRenderDrawColor(renderer, 230, 230, 240, 255);
        drawText(difficultyNames[i], r.x + r.w / 2.0f, r.y + 24.0f, 2.0f);
    }

    SDL_SetRenderDrawColor(renderer, 140, 140, 155, 255);
    drawText("W/S or mouse to choose, Enter or click to start", 640.0f, 580.0f, 1.0f);
    drawText("The number is extra enemies in every room", 640.0f, 600.0f, 1.0f);
    drawText("Esc in a run returns here, Esc here quits", 640.0f, 620.0f, 1.0f);

    SDL_RenderPresent(renderer);
}

void Game::renderInventory() {
    // Four item slots, bottom-left, labelled 1-4
    for (int i = 0; i < Inventory::Size; i++) {
        SDL_FRect slot = slotRect(i);
        const Item& s = player->getInventory().get(i);
        if (!s.isEmpty()) {
            s.render(renderer, slot);
        }
        else {
            SDL_SetRenderDrawColor(renderer, 40, 40, 50, 255);
            SDL_RenderFillRect(renderer, &slot);
        }

        // In drop mode, outline the slots that hold something you can drop
        if (dropMode && !s.isEmpty()) {
            SDL_SetRenderDrawColor(renderer, 255, 70, 70, 255);
            SDL_FRect o1{ slot.x - 2.0f, slot.y - 2.0f, slot.w + 4.0f, slot.h + 4.0f };
            SDL_FRect o2{ slot.x - 3.0f, slot.y - 3.0f, slot.w + 6.0f, slot.h + 6.0f };
            SDL_RenderRect(renderer, &o1);
            SDL_RenderRect(renderer, &o2);
        }

        SDL_SetRenderDrawColor(renderer, 200, 200, 210, 255);
        drawText(std::to_string(i + 1).c_str(), slot.x + slot.w / 2.0f, slot.y - 12.0f, 1.0f);
    }

    // Equipped weapon, to the right of the slots
    SDL_FRect wslot{ 280.0f, 650.0f, 48.0f, 48.0f };
    const Item& w = player->getWeapon();
    if (!w.isEmpty()) {
        w.render(renderer, wslot);
    }
    else {
        SDL_SetRenderDrawColor(renderer, 40, 40, 50, 255);
        SDL_RenderFillRect(renderer, &wslot);
    }
    SDL_SetRenderDrawColor(renderer, 200, 200, 210, 255);
    drawText("WEAPON", wslot.x + wslot.w / 2.0f, wslot.y - 12.0f, 1.0f);
    drawText(("DMG " + std::to_string(player->getDamage())).c_str(),
        wslot.x + wslot.w / 2.0f, wslot.y + 54.0f, 1.0f);

    // DROP button: orange while drop mode is on
    SDL_FRect db = dropButtonRect();
    if (dropMode) SDL_SetRenderDrawColor(renderer, 255, 160, 40, 255);
    else          SDL_SetRenderDrawColor(renderer, 60, 60, 75, 255);
    SDL_RenderFillRect(renderer, &db);
    if (dropMode) SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
    else          SDL_SetRenderDrawColor(renderer, 230, 230, 240, 255);
    drawText("DROP (Q)", db.x + db.w / 2.0f, db.y + 20.0f, 1.0f);

    if (dropMode) {
        SDL_SetRenderDrawColor(renderer, 255, 120, 120, 255);
        drawText("Pick a slot to drop (1-4 or click)", 405.0f, db.y - 14.0f, 1.0f);
    }
}

void Game::render() {
    if (state == GameState::Menu) {
        renderMenu();
        return;
    }

    // Background: the dungeon floor, or a flat colour if the image didn't load
    SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
    SDL_RenderClear(renderer);
    if (Assets::background) {
        SDL_RenderTexture(renderer, Assets::background, nullptr, nullptr);
    }

    for (auto& p : pickups) {
        p->render(renderer);
    }

    player->render(renderer);

    for (auto& e : enemies) {
        e->render(renderer);
    }

    if (boss) {
        boss->render(renderer);

        float bf = boss->healthFraction();
        SDL_FRect bbg{ 340.0f, 20.0f, 600.0f, 20.0f };
        SDL_FRect bbar{ 340.0f, 20.0f, 600.0f * bf, 20.0f };
        SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
        SDL_RenderFillRect(renderer, &bbg);
        SDL_SetRenderDrawColor(renderer, (Uint8)(255 * (1.0f - bf)), (Uint8)(255 * bf), 0, 255);
        SDL_RenderFillRect(renderer, &bbar);
    }

    // HUD: player health bar
    float f = player->healthFraction();
    SDL_FRect bg{ 20.0f, 20.0f, 200.0f, 16.0f };
    SDL_FRect bar{ 20.0f, 20.0f, 200.0f * f, 16.0f };
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderFillRect(renderer, &bg);
    SDL_SetRenderDrawColor(renderer, (Uint8)(255 * (1.0f - f)), (Uint8)(255 * f), 0, 255);
    SDL_RenderFillRect(renderer, &bar);

    // HUD: one square per room
    for (int i = 0; i < levelManager.getTotalLevels(); i++) {
        SDL_FRect pip{ 20.0f + i * 24.0f, 46.0f, 16.0f, 16.0f };
        if (levelManager.isFinished()) {
            SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
        }
        else if (i < levelManager.getLevel()) {
            SDL_SetRenderDrawColor(renderer, 255, 160, 40, 255);
        }
        else {
            SDL_SetRenderDrawColor(renderer, 60, 60, 70, 255);
        }
        SDL_RenderFillRect(renderer, &pip);
    }

    renderInventory();

    if (won) {
        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
        drawText("YOU WIN", 640.0f, 300.0f, 4.0f);
        SDL_SetRenderDrawColor(renderer, 230, 230, 240, 255);
        drawText("Press Enter to return to the menu", 640.0f, 400.0f, 1.0f);
    }

    SDL_RenderPresent(renderer);
}

void Game::shutdown() {
    boss.reset();
    enemies.clear();
    pickups.clear();
    player.reset();
    Assets::unload();     // textures must be freed before the renderer
    if (renderer) { SDL_DestroyRenderer(renderer); renderer = nullptr; }
    if (window) { SDL_DestroyWindow(window);     window = nullptr; }
    SDL_Quit();
}