#include "ReplayWindow.h"
#include "ui_replaywindow.h"

ReplayWindow::ReplayWindow(const QVector<Move>& moves, QWidget *parent)
    : QWidget(parent), ui(new Ui::ReplayWindow), moves(moves), moveIndex(0)
{
    ui->setupUi(this);
    setWindowTitle("Replay Game");
    setupGrid();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &ReplayWindow::playNextMove);
}

ReplayWindow::~ReplayWindow() {
    delete ui;
}

void ReplayWindow::setupGrid() {
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j) {
            QString btnName = QString("cell%1%2").arg(i).arg(j);
            cellButtons[i][j] = findChild<QPushButton*>(btnName);
            if (cellButtons[i][j]) {
                cellButtons[i][j]->setText(" ");
                cellButtons[i][j]->setEnabled(false);
            }
        }

    ui->nextMoveButton->setEnabled(true);
    ui->playAllButton->setEnabled(true);
}

void ReplayWindow::simulateMove(int row, int col, const QString& symbol) {
    if (row >= 0 && row < 3 && col >= 0 && col < 3 && cellButtons[row][col]) {
        cellButtons[row][col]->setText(symbol);
    }
}

void ReplayWindow::on_nextMoveButton_clicked() {
    if (moveIndex < moves.size()) {
        const Move& move = moves[moveIndex];
        QString symbol = (moveIndex % 2 == 0) ? "X" : "O";
        simulateMove(move.row, move.column, symbol);
        moveIndex++;
    }

    if (moveIndex >= moves.size()) {
        ui->nextMoveButton->setEnabled(false);
        ui->playAllButton->setEnabled(false);
    }
}

void ReplayWindow::playNextMove() {
    if (moveIndex < moves.size()) {
        const Move& move = moves[moveIndex];
        QString symbol = (moveIndex % 2 == 0) ? "X" : "O";
        simulateMove(move.row, move.column, symbol);
        moveIndex++;
    } else {
        timer->stop();
        ui->nextMoveButton->setEnabled(false);
        ui->playAllButton->setEnabled(false);
    }
}

void ReplayWindow::on_playAllButton_clicked() {
    timer->start(700);  // Play every 700ms
}

void ReplayWindow::on_closeButton_clicked() {
    this->close();
}
