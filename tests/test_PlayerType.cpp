#include <gtest/gtest.h>
#include <array>
#include "PlayerType.h"
#include "Model.h"

// check if X can play in open cell
TEST(HumanPlay, XPlayInOpenCell) {
    // arrange
    Model game;
    Human human;
    std::array<std::array<Cell, 3>, 3> expectedGrid;
    std::array<std::array<Cell, 3>, 3> actualGrid;
    int row = 0;
    int col = 1;
    Player player = X;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            expectedGrid[i][j] = Open;
        }
    }

    expectedGrid[row][col] = (player == X)? XCell : OCell;

    // action
    human.play(player, game, row, col);
    actualGrid = game.getGrid();
    
    // assert
    EXPECT_EQ(expectedGrid, actualGrid);
}

// check if O can play in open cell
TEST(HumanPlay, OPlayInOpenCell) {
    // arrange
    Model game;
    Human human;
    std::array<std::array<Cell, 3>, 3> expectedGrid;
    std::array<std::array<Cell, 3>, 3> actualGrid;
    int row = 2;
    int col = 2;
    Player player = O;

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            expectedGrid[i][j] = Open;
        }
    }

    expectedGrid[0][1] = XCell;
    expectedGrid[row][col] = (player == X)? XCell : OCell;
    game.play(0, 1);

    // action
    human.play(player, game, row, col);
    actualGrid = game.getGrid();
    
    // assert
    EXPECT_EQ(expectedGrid, actualGrid);
}

// check if Normal AI blocks an opponent's one move win
TEST(AIPlay, NormalBlock) {
    // arrange
    Model game;
    AI ai(Normal);
    Player player = O;
    std::array<std::array<Cell, 3>, 3> expectedGrid;
    std::array<std::array<Cell, 3>, 3> actualGrid;

    game.play(0, 0);
    game.play(1, 0);
    game.play(0, 1);
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            expectedGrid[i][j] = Open;
        }
    }

    expectedGrid[0][0] = XCell;
    expectedGrid[0][1] = XCell;
    expectedGrid[0][2] = OCell;
    expectedGrid[1][0] = OCell;

    // action
    ai.play(player, game, -1, -1);
    actualGrid = game.getGrid();

    // assert
    EXPECT_EQ(expectedGrid, actualGrid);
}

// check if Normal AI wins an one move win
TEST(AIPlay, NormalWin) {
    // arrange
    Model game;
    AI ai(Normal);
    Player player = O;
    Player winner;

    game.play(0, 0);
    game.play(1, 1);
    game.play(2, 2);
    game.play(2, 0);
    game.play(2, 1);

    // action
    ai.play(player, game, -1, -1);
    winner = game.getWinner();
    
    // assert
    EXPECT_EQ(player, winner);
}

// check if Normal AI prefer win over block
TEST(AIPlay, NormalWinOverBlock) {
    // arrange
    Model game;
    AI ai(Normal);
    Player player = O;
    Player winner;

    game.play(0, 0);
    game.play(1, 0);
    game.play(2, 0);
    game.play(1, 1);
    game.play(0, 1);

    // action
    ai.play(player, game, -1, -1);
    winner = game.getWinner();
    
    // assert
    EXPECT_EQ(player, winner);
}

// check if Hard AI blocks an opponent's one move win
TEST(AIPlay, HardBlock) {
    // arrange
    Model game;
    AI ai(Hard);
    Player player = O;
    std::array<std::array<Cell, 3>, 3> expectedGrid;
    std::array<std::array<Cell, 3>, 3> actualGrid;

    game.play(0, 0);
    game.play(1, 0);
    game.play(0, 1);
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            expectedGrid[i][j] = Open;
        }
    }

    expectedGrid[0][0] = XCell;
    expectedGrid[0][1] = XCell;
    expectedGrid[0][2] = OCell;
    expectedGrid[1][0] = OCell;

    // action
    ai.play(player, game, -1, -1);
    actualGrid = game.getGrid();

    // assert
    EXPECT_EQ(expectedGrid, actualGrid);
}

// check if Hard AI wins an one move win
TEST(AIPlay, HardWin) {
    // arrange
    Model game;
    AI ai(Hard);
    Player player = O;
    Player winner;

    game.play(0, 0);
    game.play(1, 1);
    game.play(2, 2);
    game.play(2, 0);
    game.play(2, 1);

    // action
    ai.play(player, game, -1, -1);
    winner = game.getWinner();
    
    // assert
    EXPECT_EQ(player, winner);
}

// check if Hard AI prefer win over block
TEST(AIPlay, HardWinOverBlock) {
    // arrange
    Model game;
    AI ai(Hard);
    Player player = O;
    Player winner;

    game.play(0, 0);
    game.play(1, 0);
    game.play(2, 0);
    game.play(1, 1);
    game.play(0, 1);

    // action
    ai.play(player, game, -1, -1);
    winner = game.getWinner();
    
    // assert
    EXPECT_EQ(player, winner);
}

// check if Easy AI plays
TEST(AIPlay, EasyPlay) {
    // arrange
    Model game;
    AI ai(Easy);
    Player player = X;
    int notEmpty = 0;


    // action
    ai.play(player, game, -1, -1);
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (game.getCell(i, j) != Open) {
                notEmpty++;
            }
        }
    }

    // assert
    EXPECT_EQ(notEmpty, 1);
}
