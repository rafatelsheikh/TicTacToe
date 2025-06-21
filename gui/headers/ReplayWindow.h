#ifndef REPLAYWINDOW_H
#define REPLAYWINDOW_H

#include <QWidget>
#include <QTimer>
#include <QPushButton>
#include "PlayerType.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ReplayWindow; }
QT_END_NAMESPACE

class ReplayWindow : public QWidget {
    Q_OBJECT

public:
    explicit ReplayWindow(const QVector<Move>& moves, QWidget *parent = nullptr);
    ~ReplayWindow();

private slots:
    void playNextMove();            // For Play All
    void on_nextMoveButton_clicked();
    void on_playAllButton_clicked();
    void on_closeButton_clicked();

private:
    Ui::ReplayWindow *ui;
    QVector<Move> moves;
    int moveIndex;
    QTimer* timer;

    QPushButton* cellButtons[3][3];
    void setupGrid();
    void simulateMove(int row, int col, const QString& symbol);
};

#endif // REPLAYWINDOW_H
