#ifndef SIGNUPDIALOG_H
#define SIGNUPDIALOG_H

#include <QDialog>
#include "GameDatabase.h"

namespace Ui {
class SignUpDialog;
}

class SignUpDialog : public QDialog {
    Q_OBJECT

public:
    explicit SignUpDialog(GameDatabase* db, QWidget *parent = nullptr);
    ~SignUpDialog();

private slots:
    void on_registerButton_clicked();

private:
    Ui::SignUpDialog *ui;
    GameDatabase* db;
};

#endif // SIGNUPDIALOG_H
