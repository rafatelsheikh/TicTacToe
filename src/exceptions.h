#pragma once
#include <exception>

// this is Exception to indicate that the game is already over
class IllegalStateException : public std::exception {
public:
    const char* what() const noexcept override;
};

// this is Exception to indicate that the cell can be played in
class IllegalCellException : public std::exception {
public:
    const char* what() const noexcept override;
};

// this is Exception to indicate that there is now winner yet
class NoWinnerException : public std::exception {
public:
    const char* what() const noexcept override;
};