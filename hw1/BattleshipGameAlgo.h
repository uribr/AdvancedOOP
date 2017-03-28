
#ifndef HW1_BATTLESHIPGAMEALGO_H
#define HW1_BATTLESHIPGAMEALGO_H

#include "IBattleshipGameAlgo.h"

class BattleshipGameAlgo: IBattleshipGameAlgo
{
public:
    virtual void setBoard(const char** board, int numRows, int numCols) override; // called once to notify player on his board
    virtual std::pair<int, int> attack() override; // ask player for his move
    virtual void notifyOnAttackResult(int player, int row, int col, AttackResult result) override; // notify on last move result
};

#endif //HW1_BATTLESHIPGAMEALGO_H
