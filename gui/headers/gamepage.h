#ifndef GAMEPAGE_H
#define GAMEPAGE_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include "Model.h"
#include "PlayerType.h"
#include "GameDatabase.h"
#include "GameController.h"

namespace Ui {
class GamePage;
}

class GamePage : public QWidget {
    Q_OBJECT

signals:
    void returnToMenu();

public:
    explicit GamePage(QWidget *parent,
                      GameDatabase* db,
                      const QString& player1,
                      const QString& player2,
                      bool vsAI = true,
                      Difficulty aiLevel = Normal);
    ~GamePage();
    void simulateMove(int row, int col);

private slots:
    void on_restartButton_clicked();
    void on_menuButton_clicked();
    void handleGameEnd(Status result);
    void recordMove(int row, int col);  // New slot for move recording

private:
    Ui::GamePage *ui;
    QPushButton* cellButtons[3][3];

    Model game;
    PlayerType *xPlayer;
    PlayerType *oPlayer;
    GameController* controller;
    bool isAI;
    Difficulty aiDifficulty;
    GameDatabase* db;

    QString player1;
    QString player2;
    QVector<Move> recordedMoves;

    QLabel* turnLabel;

    void updateBoard();
    void disableBoard();
    void setupMenuBar();
};

#endif // GAMEPAGE_H
