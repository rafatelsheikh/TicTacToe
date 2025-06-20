#ifndef GAME_DATABASE_H
#define GAME_DATABASE_H

#include <cstdint>
#include <string>
#include <vector>
#include <utility>
#include <SQLiteCpp/SQLiteCpp.h>
#include "PlayerType.h"
// Struct to represent a game history
struct GameHistoryEntry {
    int game_id;
    std::string opponent_type;
    std::string opponent;
    std::string outcome;
    std::string timestamp;
    std::vector<Move> game_seq;
};

class GameDatabase {
public:
    GameDatabase(const std::string& db_path);

    // Player-related methods
    bool usernameExists(const std::string& username);
    void registerPlayer(const std::string& username, const std::string& plain_password);
    bool verifyPassword(const std::string& username, const std::string& plain_password);
    std::pair<std::string, std::string> getSaltAndHash(const std::string& username);

    // Game record methods
    void recordAIGame(const std::string& player, const std::string& ai_difficulty, const std::string& outcome, const std::vector<Move>& moves);
    void recordPlayerGame(const std::string& playerA, const std::string& type, const std::string& playerB, const std::string& outcomeA, const std::vector<Move>& moves);

    // History retrieval
    std::vector<GameHistoryEntry> getGameHistory(const std::string& username);

private:
    SQLite::Database db;

    // Helper methods
    std::string generateSalt(size_t length = 16);
    std::string hashPassword(const std::string& password, const std::string& salt_hex);
    std::string serializeMoves(const std::vector<Move>& moves);
    std::vector<Move> deserializeMoves(const std::string& moves_str);
};

#endif