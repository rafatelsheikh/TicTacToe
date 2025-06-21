#include "PlayerType.h"
#include "Model.h"
#include <iostream>
#include <array>
#include <vector>
#include <limits> 
using namespace std;

void AI::playBestMove(Model& game, Player aiPlayer) {
    // initializing variables
    int bestScore = numeric_limits<int>::min();
    int bestRow = -1;
    int bestCol = -1;
    array<array<Cell, 3>, 3> grid = game.getGrid();

    // try all moves
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            // try a move
            if (grid[row][col] == Open) {
                game.play(row, col);
                game.updateStatus();

                int score = minimax(game, 0, false, numeric_limits<int>::min(), numeric_limits<int>::max(), aiPlayer);

                game.undo(row, col);

                // update the best score
                if (score > bestScore) {
                    bestScore = score;
                    bestRow = row;
                    bestCol = col;
                }
            }
        }
    }

    // play the best move
    game.play(bestRow, bestCol);
    game.updateStatus();
}

int AI::minimax(Model& game, int depth, bool maximizingPlayer, int alpha, int beta, Player player) {
    // check if the game is over and if it's over who won if any
    if (game.isTheGameOver()) {
        if (game.getStatus() == Win) {
            return (player == game.getWinner()) ? 10 - depth : depth - 10;
        }
        else if (game.getStatus() == Draw) {
            return 0;
        }
    }

    // initializing variables
    int bestScore = maximizingPlayer ? numeric_limits<int>::min() : numeric_limits<int>::max();
    array<array<Cell, 3>, 3> grid = game.getGrid();

    // explore all possible moves
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            if (grid[row][col] == Open) {
                // try a move
                game.play(row, col);
                game.updateStatus();
                int score = minimax(game, depth + 1, !maximizingPlayer, alpha, beta, player);
                game.undo(row, col);

                // update teh best score
                if (maximizingPlayer) {
                    bestScore = max(bestScore, score);
                    alpha = max(alpha, bestScore);
                }
                else {
                    bestScore = min(bestScore, score);
                    beta = min(beta, bestScore);
                }

                // purning
                if (beta <= alpha) {
                    break;
                }
            }
        }
    }

    return bestScore;
}

void AI::playEasyMove(Model& game) {
    // initializing variables
    array<array<Cell, 3>, 3> grid = game.getGrid();
    vector<Move> availableMoves;

    // get all open cells
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            if (grid[row][col] == Open) {
                Move move;
                move.row = row;
                move.column = col;
                availableMoves.emplace_back(move);
            }
        }
    }

    // play the move
    if (!availableMoves.empty()) {
        int idx = rand() % availableMoves.size();
        game.play(availableMoves[idx].row, availableMoves[idx].column);
        game.updateStatus();
    }
}

void AI::playNormalMove(Model& game, Player aiPlayer) {
    // initializing variables
    array<array<Cell, 3>, 3> grid = game.getGrid();
    Cell opponentCell = (aiPlayer == X) ? OCell : XCell;

    // check if there is an imidiate win
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            if (grid[row][col] == Open) {
                game.play(row, col);
                game.updateStatus();

                if (game.getStatus() == Win && game.getWinner() == aiPlayer) {
                    return;
                }
                
                game.undo(row, col);
            }
        }
    }

    // check if there is a block
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            if (grid[row][col] == Open) {
                grid[row][col] = opponentCell;

                if (isOpponentWin(grid, opponentCell)) {
                    game.play(row, col);
                    game.updateStatus();
                    return;
                }
                else {
                    grid[row][col] = Open;
                }
            }
        }
    }

    // play a random move
    playEasyMove(game);
}

bool AI::isOpponentWin(array<array<Cell, 3>, 3> grid, Cell winner) {
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

AI::AI(Difficulty diff) {
    difficulty = diff;
}

void AI::play(Player player, Model& game, int row, int col) {
    // identify how to play for every difficulty
    switch (difficulty) {
        case Easy:
            playEasyMove(game);
            break;
        case Normal:
            playNormalMove(game, game.whoIsNext());
            break;
        case Hard:
            playBestMove(game, game.whoIsNext());
            break;
        default:
            playNormalMove(game, game.whoIsNext());
            break;
    }
}

bool AI::isHuman() {
    return false;
}

Difficulty AI::getDifficulty() {
    return difficulty;
}