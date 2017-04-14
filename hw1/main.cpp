#include <iostream>
#include <fstream>
#include <sstream>
#include <bitset>
#include <set>
#include <map>
#include <vector>
#include <stdlib.h>
#include "inputUtilities.h"
#include "Bonus.h"

using namespace std;

#define PARAM_QUIET "-quiet"
#define PARAM_DELAY "-delay"

eShipType charToShipType(char c)
{
    switch (toupper(c))
    {
        case BOAT:
            return eShipType::SHIP_TYPE_B;
        case MISSLE_SHIP:
            return eShipType::SHIP_TYPE_P;
        case SUBMARINE:
            return eShipType::SHIP_TYPE_M;
        case DESTROYER:
            return eShipType::SHIP_TYPE_D;
        default:
            return eShipType::SHIP_TYPE_ERROR;
    }
}

int calculateSinkScore(char c)
{
	switch (toupper(c))
	{
	case BOAT:
		return BOAT_SCORE;
	case MISSLE_SHIP:
		return MISSLE_SHIP_SCORE;
	case SUBMARINE:
		return SUBMARINE_SCORE;
	case DESTROYER:
		return DESTROYER_SCORE;
	default:
		return -1;
	}
}

void changeCurrentPlayer(int *attackerNum, int *defenderNum)
{
    *attackerNum = *attackerNum ? 0: 1;
    *defenderNum = *defenderNum ? 0 : 1;
}

int main(int argc, char** argv)
{
    string dirPath;
    string atkPathA;
    string atkPathB;
    string boardPath;
    string* board = new string[ROW_SIZE];
    vector<pair<int,int>> MovesA;
    vector<pair<int,int>> MovesB;
    Player A;
    Player B;
    DWORD sleepTime = DEFAULT_SLEEP_TIME;
    bool playWithGraphics = true;
    bool gotDirPath = false;
    char **boardA = new char *[ROW_SIZE];
    char **boardB = new char *[ROW_SIZE];
    for (int i = 0; i < COL_SIZE; ++i)
    {
        boardA[i] = new char[COL_SIZE];
        boardB[i] = new char[COL_SIZE];
    }

    //processing program arguments
    if (argc >= 2)
    { // we accept the arguments in any order, and we assume that if a folder path is given it is the first argument
        for (int i = 1; i < argc; ++i)
        {
            if (!strcmp(argv[i], PARAM_QUIET))
            {
                playWithGraphics = false;
            }
            else if (!strcmp(argv[i], PARAM_DELAY))
            {
                if (i+1 < argc )
                {
                    char *p;
                    long delay = strtol(argv[i+1], &p, 10);
                    if (!*p)
                    {
                        sleepTime = (DWORD)delay;
                    }
                    // if there's no integer after PARAM_DELAY - we ignore this parameter...
                }
            }
            else if (i == 1)
            { // we assume that if there's a folder path it is the first argument
                dirPath = argv[1];
                if (searchFiles(dirPath, atkPathA, atkPathB, boardPath) < 0)
                {
                    return EXIT_FAILURE;
                }
                boardPath = dirPath + "/" + boardPath;
                atkPathA = dirPath + "/" + atkPathA;
                atkPathB = dirPath + "/" + atkPathB;
                gotDirPath = true;
            }
        }
    }
    if (!gotDirPath) // directory path given
    {
        dirPath = getDirPath();
        if (dirPath == BAD_STRING) //error occurred in getDirPath()
        {
            perror("Error");
            return EXIT_FAILURE;
        }
        if (searchFiles(dirPath, atkPathA, atkPathB, boardPath) < 0)
        {
            return EXIT_FAILURE;
        }
    }

    // setting up the main board
    initBoard(boardPath, board);
    // checking board validity
    if (checkBoardValidity(board) < 0)
    {
        return EXIT_FAILURE;
    }
    //setting up individual boards
    initIndividualBoards(board,boardA,boardB);
    // setting up attack vectors
    initAttack(atkPathA, MovesA);
    initAttack(atkPathB, MovesB);


    //now we pass the individual boards, attack vectors and ship liststo the players
    A.setBoard((const char **)boardA, ROW_SIZE, COL_SIZE);
    A.initShipsList();
    A.setMoves(MovesA);
    B.setBoard((const char **)boardB, ROW_SIZE, COL_SIZE);
    B.initShipsList();
    B.setMoves(MovesB);

    // Let the game begin!!!
    int attackerNum = 0;
	int defenderNum = 1;
    int sinkScore = 0;
    int scores[2] = {0}; // index 0 = A, index 1 = B
	Player *pPlayers[2] = { &A, &B };
    char hittenChar;
    AttackResult attackResult;
    string attackerName = "A";

    if (playWithGraphics)
    {
        // print the initial game board
        printBoard(board);
        Sleep(sleepTime);
    }
    //The game goes on until one of the players has no more ships or both ran out of moves.
    while (pPlayers[0]->hasShips() && pPlayers[1]->hasShips() && (pPlayers[0]->hasMoves() || pPlayers[1]->hasMoves()))
    {
        //Skip if current player is out of moves.
        if (!pPlayers[attackerNum]->hasMoves())
        {
            //cout << "Player " << attackerName << " has ran out of moves - SWITCHING PLAYER" << endl;
            attackerName = attackerNum ? "A" : "B";
            changeCurrentPlayer(&attackerNum, &defenderNum);
            continue;
        }

        std::pair<int, int> currentMove = pPlayers[attackerNum]->attack();
        // should always pass this check - it's for debug purposes
        if (currentMove.first < 0 || currentMove.first >= ROW_SIZE ||
                currentMove.second < 0 || currentMove.second >= COL_SIZE )
        {
            cout << "Error: Invalid move from player " << attackerName << " - (" << currentMove.first << ","
                 << currentMove.second << ")" << endl;
            return EXIT_FAILURE;
        }
        hittenChar = board[currentMove.first][currentMove.second];
        printSign(currentMove.first, currentMove.second, COLOR_RED, BOMB_SIGN, sleepTime, playWithGraphics);

        //cout << attackerName << ": (" << (currentMove.first + 1) << "," << (currentMove.second + 1) << ")" << endl;
		//cout << "char shot: $" << hittenChar << "$" << endl;
        if (hittenChar == WATER)
        {
            // Miss
			//cout << "It's a miss - no points for you, come back tomorrow - SWITCHING PLAYER" << endl;
            printSign(currentMove.first, currentMove.second, COLOR_DEFAULT_WHITE, WATER, sleepTime, playWithGraphics);
        }
        else // Hit xor Sink xor double hit xor hit a sunken ship
        {
            printSign(currentMove.first, currentMove.second, (isupper(hittenChar) ? COLOR_GREEN : COLOR_YELLOW), HIT_SIGN,
                      sleepTime, playWithGraphics);
            pPlayers[(isupper(hittenChar) ? 0 : 1)]->registerHit(currentMove, charToShipType(hittenChar), attackResult);
            //notify players on attack results
            A.notifyOnAttackResult(attackerNum, currentMove.first, currentMove.second, attackResult);
            B.notifyOnAttackResult(attackerNum, currentMove.first, currentMove.second, attackResult);
            if(attackResult == AttackResult::Sink)
            {
                //Sink
                // calculate the score
                sinkScore = calculateSinkScore(hittenChar);
                if (sinkScore == -1) // for debug - should not get here
                {
                    cout << "Error: Unexpected char on board: " << hittenChar << endl;
                    return EXIT_FAILURE;
                }
                // if hittenChar is an UPPERCASE char - than A was hit and B gets the points (and vice versa)
                scores[(isupper(hittenChar) ? 1 : 0)] += sinkScore;
                //cout << "It's a hit! The ship has sunk! Yarr!!" << endl;
                //cout << "CURRENT SCORE: A-" << scores[0] << ", B-" << scores[1] << endl;
                continue;
            }
//            else if (attackResult == AttackResult::Hit)
//            {
//                //Hit xor self hit
//                cout << "It's a" << (!isupper(hittenChar)  == attackerNum ? "self " : " ") << " hit!  Yarr!!" << (!isupper(hittenChar)  == attackerNum ? "- SWITCHING PLAYER" : "") << endl;
//                continue;
//            }
//            else
//            {
//                cout << "Double hit or hit a sunken ship - SWITCHING PLAYER" << endl;
//            }
        }
        //Change player
        attackerName = attackerNum ? "A" : "B";
        changeCurrentPlayer(&attackerNum, &defenderNum);
    }

    if(!pPlayers[0]->hasShips())
    {
        cout << "Player B won" << endl;
    }
    else if(!pPlayers[1]->hasShips())
    {
        cout << "Player A won" << endl;
    }
    cout << "Points:\nPlayer A:" << scores[0] << "\nPlayer B:" << scores[1] << endl;

    // delete all local boards
    for (int i = 0; i < COL_SIZE; ++i)
    {
        delete boardA[i];
        delete boardB[i];
    }
    delete[] boardA;
    delete[] boardB;
    delete[] board;

    return EXIT_SUCCESS;
}





