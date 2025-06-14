#include "Controller.h"
#include <iostream>
using namespace std;

Controller::Controller(PlayerType* x, PlayerType* o) {
    XType = x;
    OType = o;
}

Controller::~Controller() {
    delete XType;
    delete OType;
}

void Controller::go(Model newGame) {
    while (!newGame.isTheGameOver()) {
        // display the grid
        array<array<Cell, 3>, 3> grid = newGame.getGrid();
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                switch (grid[i][j]) {
                    case XCell:
                        cout << "X";
                        break;
                    case OCell:
                        cout << "O";
                        break;
                    case Open:
                        cout << "-";
                        break;
                    default:
                        cout << ".";
                        break;
                }
            }
            
            cout << endl;
        }

        Player current = newGame.whoIsNext(); // get the next player to play

        // play the next move
        try {
            switch (current) {
                case X:
                    XType->play(current, newGame);
                    break;
                case O:
                    OType->play(current, newGame);
                    break;
                default:
                    break;
            }
        }
        catch (const exception& e) {
            cout << e.what() << endl;
        }
    }

    // display the final grid
    array<array<Cell, 3>, 3> grid = newGame.getGrid();
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            switch (grid[i][j]) {
                case XCell:
                    cout << "X";
                    break;
                case OCell:
                    cout << "O";
                    break;
                case Open:
                    cout << "-";
                    break;
                default:
                    cout << ".";
                    break;
            }
        }
        
        cout << endl;
    }

    // getting the final result
    switch (newGame.getStatus()) {
        case Draw:
            cout << "The game ended by Draw!\n";
            break;
        case Win:
            try {
                cout << "The winner is " << ((newGame.getWinner() == X) ? "X" : "O") << "!\n";
            }
            catch (const exception& e) {
                cout << e.what() << endl;
            }
            break;
        case Playing:
            cout << "Error the game finished before end!\n";
            break;
        default:
            cout << "DAAHHH :)\n";
            break;
    }
}