#include "database.h"  
#include "picosha2.h"
#include "PlayerType.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <random>

using namespace std;

GameDatabase::GameDatabase(const string& db_path)
    : db(db_path, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE)
{
    db.exec("PRAGMA foreign_keys = ON;");
    db.exec(R"(
        CREATE TABLE IF NOT EXISTS players (
            username TEXT PRIMARY KEY,
            salt TEXT NOT NULL,
            password_hash TEXT NOT NULL
        );
    )");
    db.exec(R"(
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
        );
    )");
}

bool GameDatabase::usernameExists(const string& username) {
    SQLite::Statement query(db, "SELECT EXISTS(SELECT 1 FROM players WHERE username = ?)");
    query.bind(1, username);
    query.executeStep();
    return query.getColumn(0).getInt() != 0;
}

void GameDatabase::registerPlayer(const string& username, const string& plain_password) {
    string salt_hex = generateSalt();
    string password_hash = hashPassword(plain_password, salt_hex);
    SQLite::Statement insert(db, "INSERT INTO players (username, salt, password_hash) VALUES (?, ?, ?)");
    insert.bind(1, username);
    insert.bind(2, salt_hex);
    insert.bind(3, password_hash);
    insert.exec();
}

pair<string, string> GameDatabase::getSaltAndHash(const string& username) {
    SQLite::Statement query(db, "SELECT salt, password_hash FROM players WHERE username = ?");
    query.bind(1, username);
    if (query.executeStep()) {
        return { query.getColumn(0).getText(), query.getColumn(1).getText() };
    }
    return { "", "" };
}

bool GameDatabase::verifyPassword(const string& username, const string& plain_password) {
    auto [salt, stored_hash] = getSaltAndHash(username);
    if (salt.empty() || stored_hash.empty()) return false;
    return hashPassword(plain_password, salt) == stored_hash;
}

string GameDatabase::serializeMoves(const vector<Move>& moves) {
    string result;
    for (size_t i = 0; i < moves.size(); i++) {
        result += to_string(moves[i].row) + "," + to_string(moves[i].column);
        if (i < moves.size() - 1) result += ";";
    }
    return result;
}

vector<Move> GameDatabase::deserializeMoves(const string& moves_str) {
    vector<Move> moves;
    if (moves_str.empty()) return moves;

    stringstream ss(moves_str);
    string move;
    while (getline(ss, move, ';')) {
        size_t comma = move.find(',');
        if (comma != string::npos) {
            int row = stoi(move.substr(0, comma));
            int col = stoi(move.substr(comma + 1));
            moves.push_back({ row, col });
        }
    }
    return moves;
}

void GameDatabase::recordAIGame(const string& player, const string& ai_difficulty, const string& outcome, const vector<Move>& moves) {
    SQLite::Statement insert(db, "INSERT INTO game_history (player_username, opponent_type, ai_difficulty, outcome, moves) VALUES (?, 'ai', ?, ?, ?)");
    insert.bind(1, player);
    insert.bind(2, ai_difficulty);
    insert.bind(3, outcome);
    insert.bind(4, serializeMoves(moves));
    insert.exec();
}

void GameDatabase::recordPlayerGame(const string& playerA, const string& type, const string& playerB, const string& outcomeA, const vector<Move>& moves) {
    string outcomeB;
    SQLite::Statement insertA(db, "INSERT INTO game_history (player_username, opponent_type, opponent_username, outcome, moves) VALUES (?, 'player', ?, ?, ?)");
    insertA.bind(1, playerA);
    insertA.bind(2, playerB);
    insertA.bind(3, outcomeA);
    insertA.bind(4, serializeMoves(moves));
    insertA.exec();

    if (type != "guest") {
        if (outcomeA == "win") outcomeB = "lose";
        else if (outcomeA == "lose") outcomeB = "win";
        else outcomeB = "draw";

        SQLite::Statement insertB(db, "INSERT INTO game_history (player_username, opponent_type, opponent_username, outcome, moves) VALUES (?, 'player', ?, ?, ?)");
        insertB.bind(1, playerB);
        insertB.bind(2, playerA);
        insertB.bind(3, outcomeB);
        insertB.bind(4, serializeMoves(moves));
        insertB.exec();
    }
}

vector<GameHistoryEntry> GameDatabase::getGameHistory(const string& username) {
    vector<GameHistoryEntry> history;
    SQLite::Statement query(db, R"(
        SELECT game_id, opponent_type,
            CASE WHEN opponent_type = 'player' THEN opponent_username ELSE 'AI (' || ai_difficulty || ')' END AS opponent,
            outcome, timestamp, moves
        FROM game_history
        WHERE player_username = ?
        ORDER BY timestamp DESC
    )");
    query.bind(1, username);

    while (query.executeStep()) {
        GameHistoryEntry entry;
        entry.game_id = query.getColumn(0).getInt();
        entry.opponent_type = query.getColumn(1).getText();
        entry.opponent = query.getColumn(2).getText();
        entry.outcome = query.getColumn(3).getText();
        entry.timestamp = query.getColumn(4).getText();
        entry.game_seq = deserializeMoves(query.getColumn(5).getText());
        history.push_back(entry);
    }
    return history;
}

string GameDatabase::generateSalt(size_t length) {
    static const char hex_chars[] = "0123456789abcdef";
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 15);
    string salt;
    for (size_t i = 0; i < length * 2; ++i) {
        salt += hex_chars[dis(gen)];
    }
    return salt;
}

string GameDatabase::hashPassword(const string& password, const string& salt_hex) {
    string combined = salt_hex + password;
    return picosha2::hash256_hex_string(combined);
}
