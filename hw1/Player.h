#ifndef HW1_PLAYER_H
#define HW1_PLAYER_H

#include <queue>
#include <vector>
#include "IBattleshipGameAlgo.h"

using namespace std;

#define ROW_SIZE 10
#define COL_SIZE 10

class Player : IBattleshipGameAlgo
{
    char board[ROW_SIZE][COL_SIZE] = {{0}};
    std::queue<std::pair<int, int>> movesQueue;
    int ships = 0;


public:
    virtual void setBoard(const char **board, int numRows, int numCols) override; // called once to notify player on his board
    virtual std::pair<int, int> attack() override; // ask player for his move
    virtual void notifyOnAttackResult(int player, int row, int col, AttackResult result) override; // notify on last move result
    void setMoves(vector<pair<int,int>> moves);
    char ** getBoard();
    bool hasMoves();
};


#endif //HW1_PLAYER_H
