#include "Model.h"
#include <iostream>
#include <limits>
using namespace std;

Model::Model() {
    // setting all the cells in the grid as open
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            grid[i][j] = Open;
        }
    }

    status = Playing; // set the status as playing
    lastPlayed = O; // set the lastPlayed as O to insure that X always plays first
}

Player Model::whoIsNext() {
    // check if the game is over
    if (isTheGameOver()) {
        throw IllegalStateException();
    }

    switch (lastPlayed) {
        case X:
            return O;
            break;
        case O:
            return X;
            break;
        default:
            return X;
            break;
    } 
}

void Model::play(int row, int column) {
    // check if the cell isn't out of boundries
    if (row > 2 || row < 0 || column > 2 || column < 0) {
        throw IllegalCellException();
    }
    // check if the cell isn't empty
    else if (grid[row][column] != Open) {
        throw IllegalCellException();
    }
    
    // check who is the next player that should play to put its symbol in the chosen cell
    switch (whoIsNext()) {
        case X: 
            grid[row][column] = XCell;
            lastPlayed = X;
            break;
        case O:
            grid[row][column] = OCell;
            lastPlayed = O;
            break;
        default:
            grid[row][column] = Open;
            lastPlayed = O;
            break;
    }

    return;
}

bool Model::isTheGameOver() {
    switch (status) {
        case Playing: 
            return false;
            break;
        case Draw:
            return true;
            break;
        case Win:
            return true;
            break;
        default:
            return false;
            break;    
    }
}

array<array<Cell, 3>, 3> Model::getGrid() {
    return grid;
}

Cell Model::getCell(int row, int column) {
    // check if the cell isn't out of boundries
    if (row > 2 || row < 0 || column > 2 || column < 0) {
        throw IllegalCellException();
    }

    return grid[row][column];
}

void Model::updateStatus() {
    // check if there is a winner
    if (checkWin()) {
        status = Win;
        return;
    }
    // check if there is a draw
    if (checkDraw()) {
        status = Draw;
        return;
    }

    return;
}

bool Model::checkWin() {
    Cell winner = (lastPlayed == X) ? XCell : OCell; // put the last played player as the possible winner
    
    // check the win through rows and columns
    for (int i = 0; i < 3; i++) {
        // check if it's a row win
        if (grid[i][0] == winner && grid[i][1] == winner && grid[i][2] == winner) {
            return true;
        }
        // check if it's a column win
        else if (grid[0][i] == winner && grid[1][i] == winner && grid[2][i] == winner) {
            return true;
        }
    }

    // check if it's a diagonal win
    if (grid[0][0] == winner && grid[1][1] == winner && grid[2][2] == winner) {
        return true;
    }
    else if (grid[0][2] == winner && grid[1][1] == winner && grid[2][0] == winner) {
        return true;
    }

    return false;
}

bool Model::checkDraw() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (grid[i][j] == Open) {
                return false;
            }
        }
    }

    return !checkWin();
}

Status Model::getStatus() {
    return status;
}

Player Model::getWinner() {
    if (status == Win) {
        return lastPlayed;
    }
    else {
        throw NoWinnerException();
    }
}

void Model::undo(int row, int column) {
    grid[row][column] = Open;
    lastPlayed = (lastPlayed == X) ? O : X;
    status = Playing;
}