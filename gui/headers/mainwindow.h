#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "GameDatabase.h"
#include "signupdialog.h"
#include "PlayerType.h"
#include "SecondPlayerDialog.h"  // Add this line


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_guestButton_clicked();
    void on_signInButton_clicked();
    void on_signUpButton_clicked();
    void on_startGameButton_clicked();
    void on_aiDifficultyCombo_Toggled(bool checked);
    void openGameHistoryWindow();
    void handleLogout();
    void updateMenuState(bool loggedIn);
    void showLoginPageOnly();
    void showStatus(const QString& message);
    void startGame(bool isAI, const QString& secondPlayerName = "guest");

private:
    Ui::MainWindow *ui;
    GameDatabase* db;
    QString currentUser;
    SignUpDialog* signUpDialog = nullptr;
    SecondPlayerDialog* secondPlayerDialog = nullptr;
    QMenu* gameMenu;
    QAction* actionGameHistory;
    QAction* actionLogout;
    QAction* actionExit;

    void goToModeSelection();
};

#endif // MAINWINDOW_H
