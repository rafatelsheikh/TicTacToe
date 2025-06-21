#include "GameDatabase.h"
#include <QSqlQuery>
#include <QSqlRecord>
#include <QDebug>
#include <QRandomGenerator>

GameDatabase::GameDatabase(const QString& dbPath) {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbPath);

    if (!db.open()) {
        qWarning() << "Failed to open database:" << db.lastError().text();
        return;
    }

    QSqlQuery query;
    query.exec("PRAGMA foreign_keys = ON");

    query.exec(R"(
        CREATE TABLE IF NOT EXISTS players (
            username TEXT PRIMARY KEY,
            salt TEXT NOT NULL,
            password_hash TEXT NOT NULL
        )
    )");

    query.exec(R"(
        CREATE TABLE IF NOT EXISTS game_history (
            game_id INTEGER PRIMARY KEY AUTOINCREMENT,
            player_username TEXT NOT NULL,
            opponent_type TEXT NOT NULL CHECK(opponent_type IN ('player', 'ai')),
            opponent_username TEXT,
            ai_difficulty TEXT,
            outcome TEXT NOT NULL CHECK(outcome IN ('win', 'lose', 'draw')),
            moves TEXT,
            timestamp TEXT DEFAULT (datetime('now')),
            FOREIGN KEY (player_username) REFERENCES players(username),
            CHECK (
                (opponent_type = 'player' AND opponent_username IS NOT NULL AND ai_difficulty IS NULL AND player_username != opponent_username)
                OR
                (opponent_type = 'ai' AND opponent_username IS NULL AND ai_difficulty IS NOT NULL)
            )
        )
    )");
}

bool GameDatabase::usernameExists(const QString& username) {
    QSqlQuery query;
    query.prepare("SELECT 1 FROM players WHERE username = ?");
    query.addBindValue(username);
    if (query.exec() && query.next()) {
        return true;
    }
    return false;
}

void GameDatabase::registerPlayer(const QString& username, const QString& password) {
    QString salt = generateSalt();
    QString hash = hashPassword(password, salt);

    QSqlQuery query;
    query.prepare("INSERT INTO players (username, salt, password_hash) VALUES (?, ?, ?)");
    query.addBindValue(username);
    query.addBindValue(salt);
    query.addBindValue(hash);
    query.exec();
}

QPair<QString, QString> GameDatabase::getSaltAndHash(const QString& username) {
    QSqlQuery query;
    query.prepare("SELECT salt, password_hash FROM players WHERE username = ?");
    query.addBindValue(username);
    if (query.exec() && query.next()) {
        return { query.value(0).toString(), query.value(1).toString() };
    }
    return { "", "" };
}

bool GameDatabase::verifyPassword(const QString& username, const QString& password) {
    auto [salt, storedHash] = getSaltAndHash(username);
    if (salt.isEmpty() || storedHash.isEmpty()) return false;
    return hashPassword(password, salt) == storedHash;
}

QString GameDatabase::serializeMoves(const QVector<Move>& moves) {
    QStringList moveStrings;
    for (const Move& move : moves) {
        moveStrings.append(QString("%1,%2").arg(move.row).arg(move.column));
    }
    return moveStrings.join(';');
}

QVector<Move> GameDatabase::deserializeMoves(const QString& moves_str) {
    QVector<Move> moves;
    for (const QString& part : moves_str.split(';', Qt::SkipEmptyParts)) {
        QStringList coords = part.split(',');
        if (coords.size() == 2) {
            moves.append({ coords[0].toInt(), coords[1].toInt() });
        }
    }
    return moves;
}

void GameDatabase::recordAIGame(const QString& player, const QString& difficulty, const QString& outcome, const QVector<Move>& moves) {
    QSqlQuery query;
    query.prepare(R"(INSERT INTO game_history (player_username, opponent_type, ai_difficulty, outcome, moves)
                     VALUES (?, 'ai', ?, ?, ?))");
    query.addBindValue(player);
    query.addBindValue(difficulty);
    query.addBindValue(outcome);
    query.addBindValue(serializeMoves(moves));
    query.exec();
}

void GameDatabase::recordPlayerGame(const QString& playerA, const QString& type, const QString& playerB, const QString& outcomeA, const QVector<Move>& moves) {
    QString outcomeB;
    QSqlQuery insertA;
    insertA.prepare(R"(INSERT INTO game_history (player_username, opponent_type, opponent_username, outcome, moves)
                       VALUES (?, 'player', ?, ?, ?))");
    insertA.addBindValue(playerA);
    insertA.addBindValue(playerB);
    insertA.addBindValue(outcomeA);
    insertA.addBindValue(serializeMoves(moves));
    insertA.exec();

    if (type != "guest") {
        if (outcomeA == "win") outcomeB = "lose";
        else if (outcomeA == "lose") outcomeB = "win";
        else outcomeB = "draw";

        QSqlQuery insertB;
        insertB.prepare(R"(INSERT INTO game_history (player_username, opponent_type, opponent_username, outcome, moves)
                           VALUES (?, 'player', ?, ?, ?))");
        insertB.addBindValue(playerB);
        insertB.addBindValue(playerA);
        insertB.addBindValue(outcomeB);
        insertB.addBindValue(serializeMoves(moves));
        insertB.exec();
    }
}

QVector<GameHistoryEntry> GameDatabase::getGameHistory(const QString& username) {
    QVector<GameHistoryEntry> history;
    QSqlQuery query;
    query.prepare(R"(
        SELECT game_id, opponent_type,
               CASE WHEN opponent_type = 'player' THEN opponent_username ELSE 'AI (' || ai_difficulty || ')' END AS opponent,
               outcome, timestamp, moves
        FROM game_history
        WHERE player_username = ?
        ORDER BY timestamp DESC
    )");
    query.addBindValue(username);

    if (query.exec()) {
        while (query.next()) {
            GameHistoryEntry entry;
            entry.game_id = query.value(0).toInt();
            entry.opponent_type = query.value(1).toString();
            entry.opponent = query.value(2).toString();
            entry.outcome = query.value(3).toString();
            entry.timestamp = query.value(4).toString();
            entry.game_seq = deserializeMoves(query.value(5).toString());
            history.append(entry);
        }
    }

    return history;
}

QString GameDatabase::generateSalt(int length) {
    const QString chars = "0123456789abcdef";
    QString salt;
    for (int i = 0; i < length * 2; ++i) {
        salt.append(chars.at(QRandomGenerator::global()->bounded(chars.length())));
    }
    return salt;
}

QString GameDatabase::hashPassword(const QString& password, const QString& salt) {
    std::string combined = salt.toStdString() + password.toStdString();
    return QString::fromStdString(picosha2::hash256_hex_string(combined));
}