#ifndef GAME_DATABASE_H
#define GAME_DATABASE_H
using namespace std;
#include <string>
#include <vector>
#include <utility>
#include <SQLiteCpp/SQLiteCpp.h>
#include <string>
#include "PlayerType.h"
// Struct to represent a game history
struct GameHistoryEntry {
    int game_id;
    string opponent_type;
    string opponent;
    string outcome;
    string timestamp;
    vector<Move> game_seq;
};

class GameDatabase {
public:
    GameDatabase(const string& db_path);

    // Player-related methods
    bool usernameExists(const string& username);
    void registerPlayer(const string& username, const string& plain_password);
    bool verifyPassword(const string& username, const string& plain_password);
    pair<string, string> getSaltAndHash(const string& username);

    // Game record methods
    void recordAIGame(const string& player, const string& ai_difficulty, const string& outcome, const vector<Move>& moves);
    void recordPlayerGame(const string& playerA, const string& type, const string& playerB, const string& outcomeA, const vector<Move>& moves);

    // History retrieval
    vector<GameHistoryEntry> getGameHistory(const string& username);

private:
    SQLite::Database db;

    // Helper methods
    string generateSalt(size_t length = 16);
    string hashPassword(const string& password, const string& salt_hex);
    string serializeMoves(const vector<Move>& moves);
    vector<Move> deserializeMoves(const string& moves_str);
};

#endif