#pragma once
#include "Model.h"
#include "PlayerType.h"

// the controller who controls the input and output during the game
class Controller {
private:
    PlayerType* XType; // indicates the player type of X either Human or AI
    PlayerType* OType; // indicates the player type of O either Human or AI

public:
    // constructor to idendify the type of players that will play
    Controller(PlayerType* x = new Human(), PlayerType* o = new AI(Normal));

    // destructor to make sure the memory is deallocated
    ~Controller();

    // the gameplay
    void go(Model newGame);
};