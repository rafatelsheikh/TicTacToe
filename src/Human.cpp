#include "PlayerType.h"
#include "Model.h"
#include <iostream>
using namespace std;

void Human::play(Player player, Model& game) {
    int row, col;

    cout << "Player " << ((player == X) ? "X" : "O") << "'s turn." << endl <<"Enter the row and column: ";
    cin >> row >> col;

    game.play(row, col);
    game.updateStatus();
}