#include "exceptions.h"
#include <iostream>
using namespace std;

const char* IllegalStateException::what() const noexcept {
    return "The Game Is Already Over";
}

const char* IllegalCellException::what() const noexcept {
    return "The Cell Can't Be Played in";
}

const char* NoWinnerException::what() const noexcept {
    return "There Is No Winner Yet";
}