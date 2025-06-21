#ifndef GAMEDATABASE_H
#define GAMEDATABASE_H

#include <QString>
#include <QVector>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QVariant>
#include "picosha2.h"
#include "PlayerType.h"

// Struct for Game History Entry
struct GameHistoryEntry {
    int game_id;
    QString opponent_type;
    QString opponent;
    QString outcome;
    QString timestamp;
    QVector<Move> game_seq;
};

class GameDatabase {
public:
    GameDatabase(const QString& dbPath);
    bool usernameExists(const QString& username);
    void registerPlayer(const QString& username, const QString& password);
    bool verifyPassword(const QString& username, const QString& password);

    QString serializeMoves(const QVector<Move>& moves);
    QVector<Move> deserializeMoves(const QString& moves_str);

    void recordAIGame(const QString& player, const QString& difficulty, const QString& outcome, const QVector<Move>& moves);
    void recordPlayerGame(const QString& playerA, const QString& type, const QString& playerB, const QString& outcomeA, const QVector<Move>& moves);

    QVector<GameHistoryEntry> getGameHistory(const QString& username);

private:
    QSqlDatabase db;

    QString generateSalt(int length = 16);
    QString hashPassword(const QString& password, const QString& salt);
    QPair<QString, QString> getSaltAndHash(const QString& username);
};

#endif // GAMEDATABASE_H