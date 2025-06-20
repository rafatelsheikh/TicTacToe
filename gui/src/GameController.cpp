#include "GameController.h"
#include <QTimer>
#include "exceptions.h"

GameController::GameController(Model* model,
                               PlayerType* xPlayer,
                               PlayerType* oPlayer,
                               QObject* parent)
    : QObject(parent), m_model(model), m_xPlayer(xPlayer), m_oPlayer(oPlayer)
{}

void GameController::resetGame() {
    *m_model = Model();
    emit boardChanged();
    emit currentPlayerChanged(m_model->whoIsNext());

    // If AI goes first, trigger with delay
    if ((m_model->whoIsNext() == X && !m_xPlayer->isHuman()) ||
        (m_model->whoIsNext() == O && !m_oPlayer->isHuman())) {
        QTimer::singleShot(300, this, &GameController::triggerAIMove);
    }
}

void GameController::handleHumanMove(int row, int col) {
    if (m_model->isTheGameOver()) return;

    try {
        Player current = m_model->whoIsNext();

        if (current == X && m_xPlayer->isHuman()) {
            m_xPlayer->play(X, *m_model, row, col);
        }
        else if (current == O && m_oPlayer->isHuman()) {
            m_oPlayer->play(O, *m_model, row, col);
        }
        else {
            throw IllegalCellException();
        }

        // Signal that a move was made
        emit moveMade(row, col);

        m_model->updateStatus();
        emit boardChanged();

        if (m_model->isTheGameOver()) {
            emit gameEnded(m_model->getStatus());
        } else {
            emit currentPlayerChanged(m_model->whoIsNext());
            // Trigger AI move with delay if needed
            if ((m_model->whoIsNext() == X && !m_xPlayer->isHuman()) ||
                (m_model->whoIsNext() == O && !m_oPlayer->isHuman())) {
                QTimer::singleShot(300, this, &GameController::triggerAIMove);
            }
        }

    } catch (const std::exception& e) {
        emit invalidMoveAttempted(e.what());
    }
}

void GameController::triggerAIMove() {
    if (!m_model->isTheGameOver()) {
        playAIMove();
    }
}

void GameController::playAIMove() {
    if (m_model->isTheGameOver()) return;

    try {
        Player current = m_model->whoIsNext();

        // Save grid state BEFORE AI move
        auto grid_before = m_model->getGrid();
        int aiRow = -1, aiCol = -1;

        if (current == X && !m_xPlayer->isHuman()) {
            m_xPlayer->play(X, *m_model, -1, -1);
        }
        else if (current == O && !m_oPlayer->isHuman()) {
            m_oPlayer->play(O, *m_model, -1, -1);
        }
        else {
            return;
        }

        // Get grid AFTER AI move
        auto grid_after = m_model->getGrid();

        // Find the changed cell
        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3; c++) {
                if (grid_before[r][c] == Open && grid_after[r][c] != Open) {
                    aiRow = r;
                    aiCol = c;
                    break; // Found the move
                }
            }
            if (aiRow != -1) break; // Exit early if found
        }

        // Signal AI move
        if (aiRow != -1 && aiCol != -1) {
            emit moveMade(aiRow, aiCol);
        }

        m_model->updateStatus();
        emit boardChanged();

        if (m_model->isTheGameOver()) {
            emit gameEnded(m_model->getStatus());
        } else {
            emit currentPlayerChanged(m_model->whoIsNext());
            // Continue AI chain if needed
            if ((m_model->whoIsNext() == X && !m_xPlayer->isHuman()) ||
                (m_model->whoIsNext() == O && !m_oPlayer->isHuman())) {
                QTimer::singleShot(300, this, &GameController::triggerAIMove);
            }
        }

    } catch (const std::exception& e) {
        emit invalidMoveAttempted(e.what());
    }
}
