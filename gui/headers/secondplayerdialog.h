#ifndef SECONDPLAYERDIALOG_H
#define SECONDPLAYERDIALOG_H

#include <QDialog>
#include "GameDatabase.h"

namespace Ui {
class SecondPlayerDialog;
}

class SecondPlayerDialog : public QDialog {
    Q_OBJECT

public:
    explicit SecondPlayerDialog(GameDatabase* db, QWidget *parent = nullptr);
    ~SecondPlayerDialog();

    QString getSecondPlayer() const;

signals:
    void secondPlayerSelected(const QString& username);

private slots:
    void on_signInButton_clicked();
    void on_signUpButton_clicked();
    void on_guestButton_clicked();

private:
    Ui::SecondPlayerDialog *ui;
    GameDatabase* db;
    QString selectedUsername;
};

#endif // SECONDPLAYERDIALOG_H
