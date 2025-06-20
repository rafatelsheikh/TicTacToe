#ifndef GAMEHISTORYWINDOW_H
#define GAMEHISTORYWINDOW_H

#include <QWidget>
#include "GameDatabase.h"
#include "ReplayWindow.h"

namespace Ui {
class GameHistoryWindow;
}

class GameHistoryWindow : public QWidget {
    Q_OBJECT

public:
    explicit GameHistoryWindow(const QString& username, GameDatabase* db, QWidget *parent = nullptr);
    ~GameHistoryWindow();

private slots:
    void on_closeButton_clicked();

private:
    Ui::GameHistoryWindow *ui;
    QString currentUser;
    GameDatabase* db;

    void populateTable();
};

#endif // GAMEHISTORYWINDOW_H
