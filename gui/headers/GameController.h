#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include "Model.h"
#include "PlayerType.h"

class GameController : public QObject {
    Q_OBJECT
public:
    explicit GameController(Model* model,
                            PlayerType* xPlayer,
                            PlayerType* oPlayer,
                            QObject* parent = nullptr);

public slots:
    void handleHumanMove(int row, int col);
    void resetGame();
    void triggerAIMove();  // New slot for delayed AI move

signals:
    void boardChanged();
    void gameEnded(Status result);
    void invalidMoveAttempted(const QString& message);
    void currentPlayerChanged(Player player);
    void moveMade(int row, int col);  // New signal for move recording

private:
    void playAIMove();

    Model* m_model;
    PlayerType* m_xPlayer;
    PlayerType* m_oPlayer;
};

#endif // GAMECONTROLLER_H
