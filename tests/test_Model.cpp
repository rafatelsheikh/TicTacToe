#include <gtest/gtest.h>
#include <array>
#include "Model.h"
#include "exceptions.h"

// check if the game can't play in out of boudries cell
TEST(ModelExceptionTest, PreventOutCell) {
    // arrange
    Model game;

    // assert
    EXPECT_THROW(game.play(3, 5), IllegalCellException);
}

// check if the game can't play in already played cell
TEST(ModelExceptionTest, PreventAlreadyPlayedCell) {
    // arrange
    Model game;
    game.play(0, 0);
    game.updateStatus();

    // assert
    EXPECT_THROW(game.play(0, 0), IllegalCellException);
}

// check if required cell from getCell is out of boundries
TEST(ModelExceptionTest, PreventOutGetCell) {
    // arrange
    Model game;

    // assert
    EXPECT_THROW(game.getCell(4, 3), IllegalCellException);
}

// check if asking for who will play throw an exception if the game is already over
TEST(ModelExceptionTest, PreventKnowingWhoIsNextAfterGAme) {
    // arrange
    Model game;
    game.play(0, 0);
    game.updateStatus();
    game.play(1, 0);
    game.updateStatus();
    game.play(0, 1);
    game.updateStatus();
    game.play(1, 1);
    game.updateStatus();
    game.play(0, 2);
    game.updateStatus();

    // assert
    EXPECT_THROW(game.whoIsNext(), IllegalStateException);
}

// check if getting winner is illegal if game is still be played
TEST(ModelExceptionTest, PreventKnowingWinnerIfGameInProgress) {
    // arrange
    Model game;
    game.play(0, 0);
    game.updateStatus();

    // assert
    EXPECT_THROW(game.getWinner(), NoWinnerException);
}

// check if getting winner is illegal if game is draw
TEST(ModelExceptionTest, PreventKnowingWinnerIfDraw) {
    // arrange
    Model game;
    game.play(0, 1);
    game.updateStatus();
    game.play(0, 0);
    game.updateStatus();
    game.play(1, 0);
    game.updateStatus();
    game.play(0, 2);
    game.updateStatus();
    game.play(1, 1);
    game.updateStatus();
    game.play(1, 2);
    game.updateStatus();
    game.play(2, 0);
    game.updateStatus();
    game.play(2, 1);
    game.updateStatus();
    game.play(2, 2);
    game.updateStatus();

    // assert
    EXPECT_THROW(game.getWinner(), NoWinnerException);
}

// check if the game starts with empty grid
TEST(ModelTest, GameStartEmpty) {
    // arrange
    Model game;

    // action
    auto grid = game.getGrid();

    // assert
    for (const auto& row : grid) {
        for (const auto& cell : row) {
            ASSERT_EQ(cell, Open);
        }
    }
}

// check if the game plays in the correct cell
TEST(ModelTest, PlayInCorrectCell) {
    // arrange
    Model game;
    std::array<std::array<Cell, 3>, 3> expectedGrid;
    std::array<std::array<Cell, 3>, 3> actualGrid;
    int row = 1;
    int col = 2;
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            expectedGrid[i][j] = Open;
        }
    }

    expectedGrid[row][col] = XCell;

    // action
    game.play(row, col);
    actualGrid = game.getGrid();

    // assert
    EXPECT_EQ(expectedGrid, actualGrid);
}

// check if check draw catches the draw right
TEST(ModelTest, IsDraw) {
    // arrange
    Model game;
    bool isDraw = false;

    game.play(0, 1);
    game.updateStatus();
    game.play(0, 0);
    game.updateStatus();
    game.play(1, 0);
    game.updateStatus();
    game.play(0, 2);
    game.updateStatus();
    game.play(1, 1);
    game.updateStatus();
    game.play(1, 2);
    game.updateStatus();
    game.play(2, 0);
    game.updateStatus();
    game.play(2, 1);
    game.updateStatus();
    game.play(2, 2);
    game.updateStatus();

    // action
    isDraw = game.checkDraw();

    // assert
    EXPECT_TRUE(isDraw);
}

// check if check draw isn't only seeing if the grid is complete
TEST(ModelTest, IsNotDraw) {
    // arrange
    Model game;
    bool isDraw = false;

    game.play(0, 1);
    game.updateStatus();
    game.play(1, 0);
    game.updateStatus();
    game.play(0, 2);
    game.updateStatus();
    game.play(0, 0);
    game.updateStatus();
    game.play(2, 0);
    game.updateStatus();
    game.play(1, 1);
    game.updateStatus();
    game.play(2, 2);
    game.updateStatus();
    game.play(2, 1);
    game.updateStatus();
    game.play(1, 2);
    game.updateStatus();

    // action
    isDraw = game.checkDraw();

    // assert
    EXPECT_FALSE(isDraw);
}

// check if can be a winner if the grid is completed
TEST(ModelTest, IsWin) {
    // arrange
    Model game;
    bool isWin = false;

    game.play(0, 1);
    game.updateStatus();
    game.play(1, 0);
    game.updateStatus();
    game.play(0, 2);
    game.updateStatus();
    game.play(0, 0);
    game.updateStatus();
    game.play(2, 0);
    game.updateStatus();
    game.play(1, 1);
    game.updateStatus();
    game.play(2, 2);
    game.updateStatus();
    game.play(2, 1);
    game.updateStatus();
    game.play(1, 2);
    game.updateStatus();

    // action
    isWin = game.checkWin();

    // assert
    EXPECT_TRUE(isWin);
}

// check if the row win counts
TEST(ModelTest, RowWin) {
    // arrange
    Model game;
    Player expectedWinner = X;
    Player actualWinner;

    game.play(0, 0);
    game.updateStatus();
    game.play(1, 0);
    game.updateStatus();
    game.play(0, 1);
    game.updateStatus();
    game.play(1, 1);
    game.updateStatus();
    game.play(0, 2);
    game.updateStatus();

    // action
    actualWinner = game.getWinner();

    // assert
    EXPECT_EQ(expectedWinner, actualWinner);
}

// check if the col win counts
TEST(ModelTest, ColWin) {
    // arrange
    Model game;
    Player expectedWinner = O;
    Player actualWinner;

    game.play(0, 0);
    game.updateStatus();
    game.play(0, 1);
    game.updateStatus();
    game.play(1, 0);
    game.updateStatus();
    game.play(1, 1);
    game.updateStatus();
    game.play(0, 2);
    game.updateStatus();
    game.play(2, 1);
    game.updateStatus();

    // action
    actualWinner = game.getWinner();

    // assert
    EXPECT_EQ(expectedWinner, actualWinner);
}

// check if the diagonal win counts
TEST(ModelTest, DiagonalWin) {
    // arrange
    Model game;
    Player expectedWinner = X;
    Player actualWinner;

    game.play(0, 0);
    game.updateStatus();
    game.play(0, 1);
    game.updateStatus();
    game.play(1, 1);
    game.updateStatus();
    game.play(0, 2);
    game.updateStatus();
    game.play(2, 2);
    game.updateStatus();

    // action
    actualWinner = game.getWinner();

    // assert
    EXPECT_EQ(expectedWinner, actualWinner);
}

// check if the game starts with playing status
TEST(ModelTest, GameStartWithPlayingStatus) {
    // arrange
    Model game;

    // assert
    EXPECT_EQ(game.getStatus(), Playing);
}

// check if the game in progress has playing status
TEST(ModelTest, GameInProgressWithPlayingStatus) {
    // arrange
    Model game;

    game.play(1, 2);
    game.updateStatus();

    // assert
    EXPECT_EQ(game.getStatus(), Playing);
}

// check if the game status updated after win
TEST(ModelTest, GameWinStatus) {
    // arrange
    Model game;

    game.play(0, 0);
    game.updateStatus();
    game.play(1, 0);
    game.updateStatus();
    game.play(0, 1);
    game.updateStatus();
    game.play(1, 1);
    game.updateStatus();
    game.play(0, 2);
    game.updateStatus();
    
    // assert
    EXPECT_EQ(game.getStatus(), Win);
}

// check if the game status updated after draw
TEST(ModelTest, GameDrawStatus) {
    // arrange
    Model game;

    game.play(0, 1);
    game.updateStatus();
    game.play(0, 0);
    game.updateStatus();
    game.play(1, 0);
    game.updateStatus();
    game.play(0, 2);
    game.updateStatus();
    game.play(1, 1);
    game.updateStatus();
    game.play(1, 2);
    game.updateStatus();
    game.play(2, 0);
    game.updateStatus();
    game.play(2, 1);
    game.updateStatus();
    game.play(2, 2);
    game.updateStatus();

    // assert
    EXPECT_EQ(game.getStatus(), Draw);
}

// check if get cell returns the cell correctly
TEST(ModelTest, GetCell) {
    // arrange
    Model game;

    game.play(1, 1);
    game.updateStatus();

    // assert
    EXPECT_EQ(game.getCell(1, 1), XCell);
}

// check if it's correctly identifying who's next
TEST(ModelTest, WhoIsNext) {
    // arrange
    Model game;

    game.play(1, 1);
    game.updateStatus();

    // assert
    EXPECT_EQ(game.whoIsNext(), O);
}

// check if the game isn't over in the start
TEST(ModelTest, GameIsNotOverAtStart) {
    // arrange
    Model game;

    // assert
    EXPECT_FALSE(game.isTheGameOver());
}

// check if the game is over in the end
TEST(ModelTest, GameIsOverAtEnd) {
    // arrange
    Model game;

    game.play(0, 0);
    game.updateStatus();
    game.play(1, 0);
    game.updateStatus();
    game.play(0, 1);
    game.updateStatus();
    game.play(1, 1);
    game.updateStatus();
    game.play(0, 2);
    game.updateStatus();

    // assert
    EXPECT_TRUE(game.isTheGameOver());
}

// check if undo remove the cell choosed
TEST(ModelTest, UndoCell) {
    // arrange
    Model game;

    game.play(0, 0);
    game.updateStatus();

    // action
    game.undo(0, 0);

    // assert
    EXPECT_EQ(game.getCell(0, 0), Open);
}

// check if undo changes status
TEST(ModelTest, UndoStatus) {
    // arrange
    Model game;

    game.play(0, 0);
    game.updateStatus();
    game.play(1, 0);
    game.updateStatus();
    game.play(0, 1);
    game.updateStatus();
    game.play(1, 1);
    game.updateStatus();
    game.play(0, 2);
    game.updateStatus();

    // action
    game.undo(0, 2);

    // assert
    EXPECT_EQ(game.getStatus(), Playing);
}
