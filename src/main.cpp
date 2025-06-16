#include "Controller.h"
#include <iostream>
#include <cstdlib> 
#include <ctime>
using namespace std;

int main() {
    srand(time(nullptr));
    
    Model game;

    Controller gameController(new Human(), new AI(Hard));
    
    gameController.go(game);
}