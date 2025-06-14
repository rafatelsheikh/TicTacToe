#pragma once
#include <array>

class Model;

// Player is one of X and O, it is used for knowing which turn is this
enum Player {
    X,
    O
};

// Status is one of Playing, Draw and Win, it is used to know the game status while Playing means that the game
// is still on, Draw means the game ended by draw, Win means the game ended by wining X or O
enum Status {
    Playing,
    Draw,
    Win
};

// Cell is one of XCell, OCell, Open, it is used to know the state of a grid cell, XCell means contain x,
// OCell means contain o, Open means empty
enum Cell {
    XCell,
    OCell,
    Open
};

// Difficulty is one of Easy, Normal, Hard to identify the difficulty of the AI
enum Difficulty {
    Easy,
    Normal,
    Hard
};

// Move is a row and column of a specific move
struct Move {
    int row;
    int column;
};

// PlayerType is one of Human and AI, it's used to know the type of the player either Human or AI
class PlayerType {
public:
    // virtual function to make sure every derived class can play properly
    virtual void play(Player player, Model& game) = 0;
    virtual ~PlayerType() = default;
};

// Human is a PlayerType that indicates that this player is a human
class Human : public PlayerType {
public:
    // play as a human
    void play(Player player, Model& game);
};

// AI is a playerType that indictates that this player is AI
class AI : public PlayerType {
private: 
    Difficulty difficulty; // the difficulty of the AI

    // play the best move available
    void playBestMove(Model& game, Player aiPlayer);

    // minimax algorithm
    int minimax(Model& game, int depth, bool maximizingPlayer, int alpha, int beta, Player player);

    // play as an easy ai agent
    void playEasyMove(Model& game);

    // play as a medium ai agent
    void playNormalMove(Model& game, Player aiPlayer);

    // check if the opponent won
    bool isOpponentWin(std::array<std::array<Cell, 3>, 3> grid, Cell winner);

public:
    // initialize the AI
    AI(Difficulty diff);

    // play as AI
    void play(Player player, Model& game);
};