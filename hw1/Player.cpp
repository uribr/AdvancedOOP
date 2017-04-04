
#include "Player.h"

void Player::setBoard(const char **board, int numRows, int numCols)
{

}

// get next attack from the player's moves queue
std::pair<int, int> Player::attack()
{
    std::pair<int,int>& nextAttack = movesQueue.front();
    movesQueue.pop();
    return nextAttack;
}

void Player::notifyOnAttackResult(int player, int row, int col, AttackResult result)
{

}
