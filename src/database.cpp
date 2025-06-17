#include <iostream>
#include <iomanip>
#include <sstream>
#include "database.h"
#include "PlayerType.h"
#include "picosha2.h"


using namespace std;



// Class to manage the game database
class GameDatabase {
public:
    GameDatabase(const string& db_path)
        : db(db_path, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE)
    {
        // Enable foreign keys
        db.exec("PRAGMA foreign_keys = ON;");

        // Create players table if not exists
        db.exec(R"(
            CREATE TABLE IF NOT EXISTS players (
                username TEXT PRIMARY KEY,
                salt TEXT NOT NULL,
                password_hash TEXT NOT NULL
            );
        )");

        // Create game_history table if not exists with timestamp as the last column
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

    // Check if a username exists
    bool usernameExists(const string& username) {
        SQLite::Statement query(db, "SELECT EXISTS(SELECT 1 FROM players WHERE username = ?)");
        query.bind(1, username);
        query.executeStep();
        return query.getColumn(0).getInt() != 0;
    }

    // Register a new player with a plain password
    void registerPlayer(const string& username, const string& plain_password) {
        string salt_hex = generateSalt();
        string password_hash = hashPassword(plain_password, salt_hex);
        SQLite::Statement insert(db, "INSERT INTO players (username, salt, password_hash) VALUES (?, ?, ?)");
        insert.bind(1, username);
        insert.bind(2, salt_hex);
        insert.bind(3, password_hash);
        insert.exec();
    }

    // Retrieve the stored salt and password hash for a username
    pair<string, string> getSaltAndHash(const string& username) {
        SQLite::Statement query(db, "SELECT salt, password_hash FROM players WHERE username = ?");
        query.bind(1, username);
        if (query.executeStep()) {
            string salt = query.getColumn(0).getText();
            string hash = query.getColumn(1).getText();
            return make_pair(salt, hash);
        }
        return make_pair("", "");
    }

    // Verify password
    bool verifyPassword(const string& username, const string& plain_password) {
        auto [salt, stored_hash] = getSaltAndHash(username);
        if (salt.empty() || stored_hash.empty()) {
            return false; // User not found
        }
        string computed_hash = hashPassword(plain_password, salt);
        return computed_hash == stored_hash;
    }

    // Serialize moves vector to string
    string serializeMoves(const vector<Move>& moves) {
        string result;
        for (size_t i = 0; i < moves.size(); i++) {
            result += to_string(moves[i].row) + "," + to_string(moves[i].column);
            if (i < moves.size() - 1) result += ";";
        }
        return result;
    }

    // Deserialize moves string to vector
    vector<Move> deserializeMoves(const string& moves_str) {
        vector<Move> moves;
        if (moves_str.empty()) return moves;

        stringstream ss(moves_str);
        string move;
        while (getline(ss, move, ';')) {
            size_t comma = move.find(',');
            if (comma != string::npos) {
                int row = stoi(move.substr(0, comma));
                int col = stoi(move.substr(comma + 1));
                moves.push_back({row, col});
            }
        }
        return moves;
    }

    // Record a game against an AI opponent
    void recordAIGame(const string& player, const string& ai_difficulty, const string& outcome, const vector<Move>& moves) {
        SQLite::Statement insert(db, "INSERT INTO game_history (player_username, opponent_type, ai_difficulty, outcome, moves) VALUES (?, 'ai', ?, ?, ?)");
        insert.bind(1, player);
        insert.bind(2, ai_difficulty);
        insert.bind(3, outcome);
        insert.bind(4, serializeMoves(moves));
        insert.exec();
    }

    // Record a game between two players
    void recordPlayerGame(const string& playerA, const string& type, const string& playerB, const string& outcomeA, const vector<Move>& moves) {
        // Record for playerA
        string outcomeB;
        SQLite::Statement insertA(db, "INSERT INTO game_history (player_username, opponent_type, opponent_username, outcome, moves) VALUES (?, 'player', ?, ?, ?)");
        insertA.bind(1, playerA);
        insertA.bind(2, playerB);
        insertA.bind(3, outcomeA);
        insertA.bind(4, serializeMoves(moves));
        insertA.exec();
        if(type != "guest"){
        // Record for playerB
        if(outcomeA =="win"){
            outcomeB = "lose";
        }
        else if(outcomeA == "lose"){
            outcomeB = "win";
        }
        else{
            outcomeB = "draw";
        }
        SQLite::Statement insertB(db, "INSERT INTO game_history (player_username, opponent_type, opponent_username, outcome, moves) VALUES (?, 'player', ?, ?, ?)");
        insertB.bind(1, playerB);
        insertB.bind(2, playerA);
        insertB.bind(3, outcomeB);
        insertB.bind(4, serializeMoves(moves));
        insertB.exec();
    }
    }

    // Return each game history for a player
    vector<GameHistoryEntry> getGameHistory(const string& username) {
        vector<GameHistoryEntry> history;
        SQLite::Statement query(db, R"(
            SELECT
                game_id,
                opponent_type,
                CASE
                    WHEN opponent_type = 'player' THEN opponent_username
                    ELSE 'AI (' || ai_difficulty || ')'
                END AS opponent,
                outcome,
                timestamp,
                moves
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

private:
    SQLite::Database db;

    string generateSalt(size_t length = 16) {
        unsigned char salt[length];
        if (RAND_bytes(salt, length) != 1) {
            throw runtime_error("Failed to generate salt");
        }
        stringstream ss;
        for (size_t i = 0; i < length; i++) {
            ss << hex << setw(2) << setfill('0') << static_cast<unsigned int>(salt[i]);
        }
        return ss.str();
    }

    string hashPassword(const string& password, const string& salt_hex) {
        size_t salt_len = salt_hex.length() / 2;
        vector<unsigned char> salt(salt_len);
        for (size_t i = 0; i < salt_len; i++) {
            string byte_str = salt_hex.substr(i * 2, 2);
            salt[i] = static_cast<unsigned char>(stoi(byte_str, nullptr, 16));
        }
        SHA256_CTX sha256;
        if (SHA256_Init(&sha256) != 1) {
            throw runtime_error("Failed to initialize SHA256");
        }
        if (SHA256_Update(&sha256, salt.data(), salt.size()) != 1) {
            throw runtime_error("Failed to update SHA256 with salt");
        }
        if (SHA256_Update(&sha256, password.c_str(), password.size()) != 1) {
            throw runtime_error("Failed to update SHA256 with password");
        }
        unsigned char hash[SHA256_DIGEST_LENGTH];
        if (SHA256_Final(hash, &sha256) != 1) {
            throw runtime_error("Failed to finalize SHA256");
        }
        stringstream ss;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            ss << hex << setw(2) << setfill('0') << static_cast<unsigned int>(hash[i]);
        }
        return ss.str();
    }
};








// int main() {
//     GameDatabase db("game.db");

//     // Register players if they don’t exist
//     if (!db.usernameExists("player1")) {
//         db.registerPlayer("player1", "actual_password1");
//     } else {
//         cout << "username already exists" << endl;
//     }
//     if (!db.usernameExists("player2")) {
//         db.registerPlayer("player2", "actual_password2");
//     } else {
//         cout << "username already exists" << endl;
//     }

//     // Verify password (example)
//     bool isValid = db.verifyPassword("player1", "actual_password1");
//     cout << "Password verification for player1: " << (isValid ? "Success" : "Failure") << endl;
//     isValid = db.verifyPassword("player2", "kokowawa");
//     cout << "Password verification for player2: " << (isValid ? "Success" : "Failure") << endl;

//     // Test recording an AI game with moves
//     vector<Move> ai_moves = {{0, 0}, {1, 1}, {2, 2}}; // Example moves
//     db.recordAIGame("player1", "hard", "win", ai_moves);

//     // Test recording a player vs player game with moves
//     vector<Move> player_moves = {{0, 1}, {1, 0}, {2, 1}}; // Example moves
//     db.recordPlayerGame("player1", "guest", "im guest", "win", player_moves);
//     db.recordPlayerGame("player1", "player", "player2", "win", player_moves);

//     // Get and display game history for player1
//     auto history = db.getGameHistory("player1");
//     for (const auto& entry : history) {
//         cout << "Game ID: " << entry.game_id << ", Opponent: " << entry.opponent
//              << ", Outcome: " << entry.outcome << ", Time: " << entry.timestamp << endl;
//         cout << "Moves: ";
//         for (const auto& move : entry.game_seq) {
//             cout << "(" << move.row << "," << move.column << ") ";
//         }
//         cout << endl;
//     }
//     return 0;
// }