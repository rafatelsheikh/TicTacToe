#include "PlayerType.h"
#include "Model.h"
#include <iostream>
using namespace std;

void Human::play(Player player, Model& game, int row, int col) {
    game.play(row, col);
    game.updateStatus();
}

bool Human::isHuman() {
        return true;
}