#include "GameHistoryWindow.h"
#include "ui_gamehistorywindow.h"

GameHistoryWindow::GameHistoryWindow(const QString& username, GameDatabase* db, QWidget *parent)
    : QWidget(parent), ui(new Ui::GameHistoryWindow), currentUser(username), db(db)
{
    ui->setupUi(this);
    populateTable();

    connect(ui->closeButton, &QPushButton::clicked, this, &GameHistoryWindow::on_closeButton_clicked);

    // Make table non-editable and row-selectable only
    ui->historyTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->historyTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->historyTable->setSelectionMode(QAbstractItemView::SingleSelection);
}

GameHistoryWindow::~GameHistoryWindow() {
    delete ui;
}

void GameHistoryWindow::on_closeButton_clicked() {
    this->close();
}

void GameHistoryWindow::populateTable() {
    QVector<GameHistoryEntry> history = db->getGameHistory(currentUser);
    ui->historyTable->setRowCount(history.size());
    ui->historyTable->setColumnCount(4);
    ui->historyTable->setHorizontalHeaderLabels({"Opponent", "Result", "Date", "Replay"});
    ui->historyTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    for (int i = 0; i < history.size(); ++i) {
        const auto& entry = history[i];

        ui->historyTable->setItem(i, 0, new QTableWidgetItem(entry.opponent));
        ui->historyTable->setItem(i, 1, new QTableWidgetItem(entry.outcome));
        ui->historyTable->setItem(i, 2, new QTableWidgetItem(entry.timestamp));

        QPushButton *replayBtn = new QPushButton("Replay");
        ui->historyTable->setCellWidget(i, 3, replayBtn);

        connect(replayBtn, &QPushButton::clicked, this, [=]() {
            // Open ReplayWindow as a subwindow
            ReplayWindow *replayWin = new ReplayWindow(entry.game_seq, nullptr);
            replayWin->setAttribute(Qt::WA_DeleteOnClose);
            replayWin->setWindowModality(Qt::ApplicationModal);
            replayWin->show();
        });
    }
}
