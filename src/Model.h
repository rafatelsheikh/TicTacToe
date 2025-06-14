#pragma once
#include "exceptions.h"
#include "PlayerType.h"
#include <array>

// the game model
class Model {
private:
    std::array<std::array<Cell, 3>, 3> grid; // the grid of the game
    Status status; // the status of the game
    Player lastPlayed; // contains the last player played

public:
    // constructor to initialize the first state of the game
    Model();

    // return the player that should play next either X or O
    Player whoIsNext();

    // play in the grid using the given row and column
    void play(int row, int column);

    // check of the game is already over
    bool isTheGameOver();

    // get the recent grid
    std::array<std::array<Cell, 3>, 3> getGrid();

    // get the cell at position [row, column]
    Cell getCell(int row, int column);

    // update the game status if there is already draw or anyone win
    void updateStatus();

    // check if it's a win case
    bool checkWin();

    // check if it's a draw case
    bool checkDraw();

    // return the game status
    Status getStatus();

    // return the winner player if any
    Player getWinner();

    // undo the last played move using its row and column
    void undo(int row, int column);
};