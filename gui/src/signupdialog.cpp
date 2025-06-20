#include "signupdialog.h"
#include "ui_signupdialog.h"
#include <QTimer>

SignUpDialog::SignUpDialog(GameDatabase* db, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SignUpDialog)
    , db(db)
{
    ui->setupUi(this);
    ui->feedbackLabel->clear();  // start with no message
}

SignUpDialog::~SignUpDialog() {
    delete ui;
}

void SignUpDialog::on_registerButton_clicked() {
    QString username = ui->usernameEdit->text().trimmed();
    QString password = ui->passwordEdit->text();
    QString confirm  = ui->confirmPasswordEdit->text();

    if (username.isEmpty() || password.isEmpty() || confirm.isEmpty()) {
        ui->feedbackLabel->setText("All fields are required.");
        return;
    }

    if (password != confirm) {
        ui->feedbackLabel->setText("Passwords do not match.");
        return;
    }

    if (db->usernameExists(username)) {
        ui->feedbackLabel->setText("Username already exists.");
        return;
    }

    try {
        db->registerPlayer(username, password);
        ui->feedbackLabel->setText("Signup successful!");

        // Wait 2 seconds before closing
        QTimer::singleShot(500, this, SLOT(close()));
    } catch (const std::exception& e) {
        ui->feedbackLabel->setText("Registration failed.");
    }
}
