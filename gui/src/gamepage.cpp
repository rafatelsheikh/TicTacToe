#include "gamepage.h"
#include "ui_gamepage.h"
#include "GameController.h"
#include <QTimer>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QAction>
#include <QLabel>
#include <QDebug>

GamePage::GamePage(QWidget *parent,
                   GameDatabase* db,
                   const QString& player1,
                   const QString& player2,
                   bool vsAI,
                   Difficulty aiLevel)
    : QWidget(parent),
    ui(new Ui::GamePage),
    isAI(vsAI),
    aiDifficulty(aiLevel),
    db(db),
    player1(player1),
    player2(player2)
{
    ui->setupUi(this);

    // Create players
    xPlayer = new Human();
    oPlayer = isAI ? static_cast<PlayerType*>(new AI(aiLevel))
                   : static_cast<PlayerType*>(new Human());

    // Find cell buttons
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j) {
            QString btnName = QString("cell%1%2").arg(i).arg(j);
            cellButtons[i][j] = findChild<QPushButton*>(btnName);
        }

    // Create turn label
    turnLabel = new QLabel("Player X's turn", this);
    turnLabel->setAlignment(Qt::AlignCenter);
    QFont font = turnLabel->font();
    font.setPointSize(12);
    turnLabel->setFont(font);
    ui->verticalLayout->insertWidget(0, turnLabel);


    // Create controller
    controller = new GameController(&game, xPlayer, oPlayer, this);

    // Connect controller signals
    connect(controller, &GameController::boardChanged,
            this, &GamePage::updateBoard);
    connect(controller, &GameController::gameEnded,
            this, &GamePage::handleGameEnd);
    connect(controller, &GameController::invalidMoveAttempted,
            this, [this](const QString& msg){ turnLabel->setText(msg); });
    connect(controller, &GameController::currentPlayerChanged,
            this, [this](Player p){
                turnLabel->setText(QString("Player %1's turn").arg(p == X ? "X" : "O"));
            });
    connect(controller, &GameController::moveMade,
            this, &GamePage::recordMove);  // Connect move recording

    // Connect buttons to controller
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            connect(cellButtons[i][j], &QPushButton::clicked,
                    this, [=](){ controller->handleHumanMove(i, j); });
        }
    }

    setupMenuBar();
    updateBoard();
}

GamePage::~GamePage() {
    delete xPlayer;
    delete oPlayer;
    delete controller;
    delete ui;
}

void GamePage::setupMenuBar() {
    QMenuBar *menuBar = new QMenuBar(this);

    menuBar->setStyleSheet(R"(
    QMenuBar {
        background-color: transparent;
        color: white;
    }
    QMenuBar::item {
        background: transparent;
        color: white;
        padding: 5px 15px;
    }
    QMenuBar::item:selected {
        background: #444;
    }
    QMenu {
        background-color: #222;
        color: white;
    }
    QMenu::item:selected {
        background-color: #444;
    }
    )");

    QMenu *menu = new QMenu("⚙", this);
    menu->setStyleSheet("color: white;");

    QAction *mainMenuAction = new QAction(QIcon(":/img/img/mainmenu.png"), "Main Menu", this);
    QAction *restartAction = new QAction(QIcon(":/img/img/restart.png"), "Restart", this);

    mainMenuAction->setIconVisibleInMenu(true);
    restartAction->setIconVisibleInMenu(true);

    connect(mainMenuAction, &QAction::triggered, this, &GamePage::on_menuButton_clicked);
    connect(restartAction, &QAction::triggered, this, &GamePage::on_restartButton_clicked);

    menu->addAction(mainMenuAction);
    menu->addAction(restartAction);

    menuBar->addMenu(menu);
    ui->verticalLayout->insertWidget(0, menuBar);
}

void GamePage::updateBoard() {
    auto grid = game.getGrid();
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (grid[i][j] == XCell) {
                cellButtons[i][j]->setText("X");
            } else if (grid[i][j] == OCell) {
                cellButtons[i][j]->setText("O");
            } else {
                cellButtons[i][j]->setText(" ");
            }
        }
    }
}

void GamePage::recordMove(int row, int col) {
    recordedMoves.append({row, col});
}

void GamePage::handleGameEnd(Status result) {
    QString outcome;
    if (result == Win) {
        Player winner = game.getWinner();
        outcome = (winner == X) ? "win" : "lose";
        turnLabel->setText(QString("Player %1 wins!").arg(winner == X ? "X" : "O"));
    } else {
        outcome = "draw";
        turnLabel->setText("It's a draw!");
    }

    disableBoard();

    // Record game in database
    if (db) {
        if (isAI) {
            db->recordAIGame(player1,
                             (dynamic_cast<AI*>(oPlayer))->getDifficulty() == Easy ? "Easy" :
                                 (dynamic_cast<AI*>(oPlayer))->getDifficulty() == Normal ? "Normal" : "Hard",
                             outcome, recordedMoves);
        } else {
            QString outcomeP2;
            if (outcome == "win") outcomeP2 = "lose";
            else if (outcome == "lose") outcomeP2 = "win";
            else outcomeP2 = "draw";

            db->recordPlayerGame(player1, player2 == "guest" ? "guest" : "player",
                                 player2, outcome, recordedMoves);
        }
    }
}

void GamePage::on_restartButton_clicked() {
    controller->resetGame();
    recordedMoves.clear();

    // Re-enable board
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            cellButtons[i][j]->setEnabled(true);

    // Update UI
    turnLabel->setText("Player X's turn");
    updateBoard();
}

void GamePage::on_menuButton_clicked() {
    emit returnToMenu();
}

void GamePage::disableBoard() {
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            cellButtons[i][j]->setEnabled(false);
}

void GamePage::simulateMove(int row, int col) {
    controller->handleHumanMove(row, col);
}
