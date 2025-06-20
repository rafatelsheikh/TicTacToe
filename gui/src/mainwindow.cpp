#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gamepage.h"
#include "GameHistoryWindow.h"
#include "SecondPlayerDialog.h"
#include <QDebug>
#include <QCoreApplication>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    db(new GameDatabase(QCoreApplication::applicationDirPath() + "/game.db"))
{
    ui->setupUi(this);

    connect(ui->vsAIRadio, &QRadioButton::toggled, this, &MainWindow::on_aiDifficultyCombo_Toggled);
    connect(ui->signInButton, &QPushButton::clicked, this, &MainWindow::on_signInButton_clicked);
    connect(ui->startGameButton, &QPushButton::clicked, this, &MainWindow::on_startGameButton_clicked);
    connect(ui->signUpButton, &QPushButton::clicked, this, &MainWindow::on_signUpButton_clicked);

    ui->aiDifficultyCombo->setEnabled(false);

    gameMenu = new QMenu(this);
    gameMenu->setTitle(QString::fromUtf8("\u2630"));  // ☰
    menuBar()->addMenu(gameMenu);

    // Game History
    actionGameHistory = new QAction(QIcon(":/img/img/history.png"), "Game History", this);
    connect(actionGameHistory, &QAction::triggered, this, &MainWindow::openGameHistoryWindow);
    gameMenu->addAction(actionGameHistory);

    // Logout
    actionLogout = new QAction(QIcon(":/img/img/logout.jpg"), "Logout", this);
    connect(actionLogout, &QAction::triggered, this, &MainWindow::handleLogout);
    gameMenu->addAction(actionLogout);

    // Exit
    actionExit = new QAction(QIcon(":/img/img/exit.jpeg"), "Exit", this);
    connect(actionExit, &QAction::triggered, this, &QApplication::quit);
    gameMenu->addAction(actionExit);

    showLoginPageOnly();
}

MainWindow::~MainWindow() {
    delete db;
    delete ui;
}

void MainWindow::showLoginPageOnly() {
    ui->stackedWidget->setCurrentWidget(ui->loginPage);
    updateMenuState(false);
    currentUser.clear();
    showStatus("Welcome. Please log in.");
}

void MainWindow::updateMenuState(bool loggedIn) {
    menuBar()->setVisible(loggedIn);
    gameMenu->setEnabled(loggedIn);
}

void MainWindow::showStatus(const QString& message) {
    statusBar()->showMessage(message, 5000);
}

void MainWindow::on_aiDifficultyCombo_Toggled(bool checked) {
    ui->aiDifficultyCombo->setEnabled(checked);
}

void MainWindow::on_guestButton_clicked() {
    currentUser = "guest";
    goToModeSelection();
    updateMenuState(true);
    showStatus("Logged in as guest.");
}

void MainWindow::on_signInButton_clicked() {
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        showStatus("Username and password cannot be empty.");
        return;
    }

    if (!db->usernameExists(username)) {
        showStatus("Username does not exist.");
        return;
    }

    if (!db->verifyPassword(username, password)) {
        showStatus("Incorrect password.");
        return;
    }

    currentUser = username;
    goToModeSelection();
    updateMenuState(true);
    showStatus("Hi, " + username + "!");
}

void MainWindow::on_signUpButton_clicked() {
    if (signUpDialog == nullptr) {
        signUpDialog = new SignUpDialog(db, this);
        connect(signUpDialog, &QDialog::finished, this, [=]() {
            signUpDialog->deleteLater();
            signUpDialog = nullptr;
        });
        signUpDialog->show();
    } else {
        signUpDialog->raise();
        signUpDialog->activateWindow();
    }
}

void MainWindow::goToModeSelection() {
    ui->stackedWidget->setCurrentWidget(ui->modeSelectionPage);
}

void MainWindow::on_startGameButton_clicked() {
    if (!ui->vsPlayerRadio->isChecked() && !ui->vsAIRadio->isChecked()) {
        showStatus("Please select a game mode.");
        return;
    }

    bool isAI = ui->vsAIRadio->isChecked();

    if (isAI) {
        startGame(true);  // Single player: start directly
    } else {
        if (!secondPlayerDialog) {
            secondPlayerDialog = new SecondPlayerDialog(db, this);
            connect(secondPlayerDialog, &SecondPlayerDialog::secondPlayerSelected, this, [=](const QString& secondPlayerName) {
                secondPlayerDialog->close();
                secondPlayerDialog->deleteLater();
                secondPlayerDialog = nullptr;

                startGame(false, secondPlayerName);  // Pass second player's name
            });
            secondPlayerDialog->show();
        } else {
            secondPlayerDialog->raise();
            secondPlayerDialog->activateWindow();
        }
    }
}


void MainWindow::startGame(bool isAI, const QString& secondPlayerName)
{
    Difficulty level = Normal;
    if (ui->aiDifficultyCombo->currentText() == "Easy") level = Easy;
    else if (ui->aiDifficultyCombo->currentText() == "Hard") level = Hard;

    GamePage* gamePage = new GamePage(this, db, currentUser, secondPlayerName, isAI, level);
    ui->stackedWidget->addWidget(gamePage);
    ui->stackedWidget->setCurrentWidget(gamePage);

    connect(gamePage, &GamePage::returnToMenu, this, [=]() {
        ui->stackedWidget->setCurrentIndex(1);
    });
}


void MainWindow::handleLogout() {
    showLoginPageOnly();
    showStatus("Logged out successfully.");
}

void MainWindow::openGameHistoryWindow() {
    if (currentUser.isEmpty() || currentUser == "guest") {
        showStatus("Guest users have no game history.");
        return;
    }

    GameHistoryWindow* window = new GameHistoryWindow(currentUser, db);
    window->setAttribute(Qt::WA_DeleteOnClose);
    window->show();
}


