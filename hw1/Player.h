
#ifndef HW1_PLAYER_H
#define HW1_PLAYER_H


#include <queue>
#include "IBattleshipGameAlgo.h"

#define BOARD_SIZE 10

class Player : IBattleshipGameAlgo
{
    char board[BOARD_SIZE][BOARD_SIZE] = {{0}};
    std::queue<std::pair<int, int>> movesQueue;
    int ships;

public:
    virtual void
    setBoard(const char **board, int numRows, int numCols) override; // called once to notify player on his board
    virtual std::pair<int, int> attack() override; // ask player for his move
    virtual void notifyOnAttackResult(int player, int row, int col, AttackResult result) override; // notify on last move result
};


#endif //HW1_PLAYER_H
