#include "Game.h"

int main(int argc, char* argv[]) {
    Game game;
    if (!game.init("Hunter", 1280, 720)) {
        return 1;
    }
    game.run();
    return 0;
}