# TicTacToe

## 🧠 Overview

This project is a version of the classic TicTacToe game developed using C++ with user-friendly GUI, AI opponent with levels, and secure user authentication system. It supports both player vs player and player vs AI modes, with persistent game history and performance tracking. the files above is divided into src files that contains the game logic and the gui contains GUI and database implementation. The zip file contain the game. You can download it and decompress the file and try to play the game.


---

## 🎯 Project objectives

- Develop a feature-rich TicTacToe game using **C++** and **Qt**.
- Enable secure **user registration, login**, and **profile management**.
- Implement an **AI opponent** with deifferent levels of difficulty.
- See **individual game history** per user showing the matches played "the opponent and the winner" and when .
- Conduct comprehensive **unit/integration testing** with Google Test.
- Use **CI/CD pipelines** with GitHub Actions for continuous testing and deployment.

---

## 🛠️ Core features

### 🎮 Game logic
- Turn-based mechanics for both multiplayer and AI matches.
- Win/tie detection after every move.

### 🧠 AI opponent
- Implements levels of difficulty easy, normal and hard.
  - **Easy:** plays random moves on the empty grid.
  - **Normal:** checks if there is a potential winning move and plays it and if there is a potentials winning move for the opponent it blocks it but it gives the priority to the winnig move. Otherwise, it plays randomly.
  - **Hard:** plays according to the Minimax with alpha-beta pruning algorithm. 

### 💻 GUI interface
- Built with Qt for rich, interactive user experience.
- GUI components for gameboard, login/register screens and history browser.

### 🔐 User Authentication
- Secure login/registration using hashed passwords.
- Session management for persistent login.

### 📜 Game history
- Logs all games played by each user showing:
  - who played the match
  - who won the match
  - when the match played
- Supports reviewing and replaying previous games.

### ✅ Testing and QA
- Unit and integration tests using **Google Test**.
- Test reports with coverage analysis.

### ⚙️ CI/CD integration
- GitHub Actions for automatic build, test, and deployment pipelines.

### 📈 Performance metrics
- Focus on minimizing latency and optimizing resource usage.

---

##📄 List of Documents
- Software Requirements Specification (SRS)
- Software Design Specification (SDS)
- Testing Document
- Performance Measurement and Optimization



---
## 🔧 Tech stack

| Component            | Technology                     |
|---------------------|---------------------------------|
| Language            | C++                             |
| GUI Framework       | Qt                              |
| Database            | SQLite                          |
| Testing Framework   | Google Test                     |
| Version Control     | Git + GitHub                    |
| CI/CD               | GitHub Actions                  |
| Security            | Password hashing                |

---

## 📚 Data structures used

- **Tree:** for AI decision-making via Minimax algorithm in hard level.
- **Struct:** for saving the moves on the grid.
- **Pairs:** to save the salt and hash password in the database.
- **Vectors:** used in the database and it behaves as a queue to save moves of a certain game.
- **Classes:** to determine the play mode human vs human or human vs AI.
- **Enums:** for representing the state of each cell or the game status—for example, PLAYER_X and PLAYER_O instead of numeric values to improves code readability and reduces logic errors. 

---
