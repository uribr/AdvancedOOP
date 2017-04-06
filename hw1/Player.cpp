#include "Player.h"

void Player::setBoard(const char **board, int numRows, int numCols)
{
    for (int i = 0; i < numRows; ++i)
    {
        for (int j = 0; j < numCols; ++j)
        {
            this->board[i][j] = board[i][j];
        }
    }

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

void Player::setMoves(vector<pair<int, int>> moves)
{
    for (int i = 0; i < moves.size(); ++i)
    {
        this->movesQueue.push(moves[i]);
    }
    moves.clear();
}

bool Player::hasMoves()
{
    return !movesQueue.empty();
}

char ** Player::getBoard()
{
    char **retBoard = new char *[ROW_SIZE];
    for (int i = 0; i < ROW_SIZE; ++i)
    {
        retBoard[i] = new char[COL_SIZE];
        for (int j = 0; j < COL_SIZE; ++j)
        {
            retBoard[i][j] = this->board[i][j];
        }
    }
    return retBoard;
}

