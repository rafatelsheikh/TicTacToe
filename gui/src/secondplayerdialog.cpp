#include "SecondPlayerDialog.h"
#include "ui_secondplayerdialog.h"
#include "signupdialog.h"
#include <QTimer>

SecondPlayerDialog::SecondPlayerDialog(GameDatabase* db, QWidget *parent)
    : QDialog(parent),
    ui(new Ui::SecondPlayerDialog),
    db(db)
{
    ui->setupUi(this);
    selectedUsername = "guest";  // Default fallback
    ui->feedbackLabel->clear();  // Clear feedback initially
}

SecondPlayerDialog::~SecondPlayerDialog() {
    delete ui;
}

QString SecondPlayerDialog::getSecondPlayer() const {
    return selectedUsername;
}

void SecondPlayerDialog::on_signInButton_clicked() {
    QString username = ui->usernameEdit->text().trimmed();
    QString password = ui->passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        ui->feedbackLabel->setText("Username and password cannot be empty.");
        return;
    }

    if (!db->usernameExists(username)) {
        ui->feedbackLabel->setText("Username does not exist.");
        return;
    }

    if (!db->verifyPassword(username, password)) {
        ui->feedbackLabel->setText("Incorrect password.");
        return;
    }

    selectedUsername = username;
    ui->feedbackLabel->setText("Signed in successfully!");
    emit secondPlayerSelected(username);
    QTimer::singleShot(500, this, &QDialog::accept);  // Close after 0.5s
}

void SecondPlayerDialog::on_signUpButton_clicked() {
    SignUpDialog signup(db, this);
    signup.exec();
}

void SecondPlayerDialog::on_guestButton_clicked() {
    selectedUsername = "guest";
    ui->feedbackLabel->setText("Continuing as guest...");
    emit secondPlayerSelected("guest");
    QTimer::singleShot(300, this, &QDialog::accept);  // Close after short delay
}
