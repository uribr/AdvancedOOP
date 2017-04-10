#include <iostream>
#include <fstream>
#include <sstream>
#include <direct.h>
#include <bitset>
#include <set>
#include <map>
#include <vector>
#include <stdlib.h>
#include "inputUtilities.h"
#include "Player.h"
#include "Bonus.h"

using namespace std;

#define PARAM_QUIET "-quiet"
#define PARAM_DELAY "-delay"


void initIndividualBoards(string *pString, char **a, char **boardB);

void printBoard(const char **board)
{
    for (int i = 0; i < ROW_SIZE; ++i)
    {
        cout << "|";
        for (int j = 0; j < COL_SIZE; ++j)
        {
            cout << board[i][j];

        }
        cout << "|" << endl;

    }
}

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

int charToSinkSCore(char c)
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
    //TODO - add some readl file loading
    string dirPath;
    string atkPathA;
    string atkPathB;
    string boardPath;
    string* board = new string[ROW_SIZE];
    vector<pair<int,int>> attackA;
    vector<pair<int,int>> attackB;
    Player A;
    Player B;
    DWORD sleepTime = DEFAULT_SLEEP_TIME;
    bool playWithGraphics = true;
    char **boardA = new char *[ROW_SIZE];
    char **boardB = new char *[ROW_SIZE];
    for (int i = 0; i < COL_SIZE; ++i)
    {
        boardA[i] = new char[COL_SIZE];
        boardB[i] = new char[COL_SIZE];
    }

    if (argc == 1)
    {
        dirPath = getDirPath();
        if (dirPath == "!@#") //error occurred in getDirPath()
        {
            perror("Error");
            return EXIT_FAILURE;
        }
        if (searchFiles(dirPath, atkPathA, atkPathB, boardPath) < 0)
        {
            return -1;
        }
    }
    else if (argc >= 2)
    {
        dirPath = argv[1];
        if (searchFiles(dirPath, atkPathA, atkPathB, boardPath) < 0)
        {
            return -1;
        }
        boardPath = dirPath + "/" + boardPath;
        atkPathA = dirPath + "/" + atkPathA;
        atkPathB = dirPath + "/" + atkPathB;

        for (int i = 2; i < argc; ++i)
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
                    if (*p)
                    {
                        cout << "Error: expected an integer after " << PARAM_DELAY << " but got " << argv[i+1] << endl;
                        return EXIT_FAILURE;
                    }
                    sleepTime = (DWORD)delay;

                }
            }
        }
    }

    // setting up the main board
    initBoard(boardPath, board);
    // checking board validity
    if (checkBoardValidity(board) < 0)
    {
        return -1;
    }
    //setting up individual boards
    initIndividualBoards(board,boardA,boardB);
    // setting up attack vectors
    initAttack(atkPathA, attackA);
    initAttack(atkPathB, attackB);


    //now we pass the individual boards, attack vectors and ship liststo the players
    A.setBoard((const char **)boardA, ROW_SIZE, COL_SIZE);
    A.initShipsList();
    A.setMoves(attackA);
    B.setBoard((const char **)boardB, ROW_SIZE, COL_SIZE);
    B.initShipsList();
    B.setMoves(attackB);

    // Let the game begin!!!
    int attackerNum = 0;
	int defenderNum = 1;
    int currentScore = 0;
    int scores[2] = {0}; // index 0 = A, index 1 = B
	Player *pPlayers[2] = { &A, &B };

    char c;
    AttackResult attackResult;
	//Player *pCurrentPlayer = &A;
    string attackerName = "A";
    // todo - delte all debug prints!!!!

    if (playWithGraphics)
    {
        system("cls");
        //print board
        for (int j = 0; j < COL_SIZE; ++j)
        {
            for (int k = 0; k < COL_SIZE; ++k)
            {
                c = board[j][k];
                setTextColor(isupper(c) ? COLOR_GREEN : COLOR_YELLOW);
                cout << c;
            }
            cout << endl;
        }
        Sleep(sleepTime);
    }
    //The game goes on until one of the players has no more ships or both ran out of moves.
    while (pPlayers[0]->hasShips() && pPlayers[1]->hasShips() && (pPlayers[0]->hasMoves() || pPlayers[1]->hasMoves()))
    {
        //Skip if current player is out of moves.
        if (!pPlayers[attackerNum]->hasMoves())
        {
            //cout << "Player " << attackerName << " has ran out of moves - SWITCHING PLAYER" << endl;
            attackerName = attackerNum ? "A" : "B"; //todo - delete this (for debug)
            changeCurrentPlayer(&attackerNum, &defenderNum);
            continue;
        }

        // should always pass this check - it's for debug purposes
        std::pair<int, int> currentMove = pPlayers[attackerNum]->attack();
        if (currentMove.first < 0 || currentMove.first >= ROW_SIZE ||
                currentMove.second < 0 || currentMove.second >= COL_SIZE )
        {
            cout << "Error: Invalid move from player " << attackerName << " - (" << currentMove.first << ","
                 << currentMove.second << ")" << endl;
            return EXIT_FAILURE;
        }

        c = board[currentMove.first][currentMove.second];

        printSign(currentMove.first, currentMove.second, COLOR_RED, BOMB_SIGN, sleepTime, playWithGraphics);

        // todo - delete this (debug) - in this printing we see the ORIGINAL coordinates (without the (-1) offset)
        //cout << attackerName << ": (" << (currentMove.first + 1) << "," << (currentMove.second + 1) << ")" << endl;
		//cout << "char shot: $" << c << "$" << endl;
        if (c == WATER)
        {
            // Miss
			//cout << "It's a miss - no points for you, come back tomorrow - SWITCHING PLAYER" << endl;
            printSign(currentMove.first, currentMove.second, COLOR_DEFAULT_WHITE, WATER, sleepTime, playWithGraphics);
        }
        else // Hit xor Sink xor double hit xor hit a sunken ship
        {
            printSign(currentMove.first, currentMove.second, (isupper(c) ? COLOR_GREEN : COLOR_YELLOW), HIT_SIGN,
                      sleepTime, playWithGraphics);
            pPlayers[(isupper(c) ? 0 : 1)]->registerHit(currentMove, charToShipType(c), attackResult);
            //notify players on attack results
            A.notifyOnAttackResult(attackerNum, currentMove.first, currentMove.second, attackResult);
            B.notifyOnAttackResult(attackerNum, currentMove.first, currentMove.second, attackResult);
            if(attackResult == AttackResult::Sink)
            {
                //Sink
                // calculate the score
                currentScore = charToSinkSCore(c);
                if (currentScore == -1)
                {
                    cout << "Error: Unexpected char on board: " << c << endl;
                    return EXIT_FAILURE;
                }
                // if c is an UPPERCASE char - than A was hit and B gets the points (and vice versa)
                scores[(isupper(c) ? 1 : 0)] += currentScore;
                //cout << "It's a hit! The ship has sunk! Yarr!!" << endl;
                //cout << "CURRENT SCORE: A-" << scores[0] << ", B-" << scores[1] << endl;
                continue;
            }
//            else if (attackResult == AttackResult::Hit)
//            {
//                //Hit xor self hit
//                cout << "It's a" << (!isupper(c)  == attackerNum ? "self " : " ") << " hit!  Yarr!!" << (!isupper(c)  == attackerNum ? "- SWITCHING PLAYER" : "") << endl;
//                continue;
//            }
//            else
//            {
//                cout << "Double hit or hit a sunken ship - SWITCHING PLAYER" << endl;
//            }
        }
        //Change player
        attackerName = attackerNum ? "A" : "B"; //todo - delete this (for debug)
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


    /* ------------------------------ PRINT TESTS ------------------------------
    // Print board
    for (int i = 0; i < rows; ++i)
    {
        for (int j =0; j < cols; ++j)
        {
            cout << " | ";
            cout << board[i][j];
        }
        cout << " | ";
        cout << endl;
    }
    // Check board validity and print errors
    cout << "check = " << checkBoardValidity(board) << endl;
    // Print attack vector
    for (int i = 0; i < attackA.size(); i++) {
        cout << attackA[i].first << "," << attackA[i].second << endl;
    }
     ------------------------------------------------------------------------- */

//    char **boardATest = A.getBoard();
//    char **boardBTest = B.getBoard();
//    printBoard((const char **)boardATest);
//    cout << endl;
//    printBoard((const char **)boardBTest);

    //TODO - is there anything else that needs to be released/destroyed?
    // delete local individual boards
    for (int i = 0; i < COL_SIZE; ++i)
    {
        delete boardA[i];
        delete boardB[i];
    }
    delete[] boardA;
    delete[] boardB;
    delete[] board;

    return 0;
}



void initIndividualBoards(string *board, char **boardA, char **boardB)
{
    char c;
    for (int i = 0; i < ROW_SIZE; ++i)
    {
        for (int j = 0; j < COL_SIZE; ++j)
        {
            c = board[i][j];
            if (isalpha(c)) //part of a ship
            {
                if (isupper(c)) // a ship of A
                {
                    boardA[i][j] = c;
                    boardB[i][j] = WATER;
                }
                else // a ship of B
                {
                    boardA[i][j] = WATER;
                    boardB[i][j] = c;
                }
            }
            else // a space - update both boards
            {
                boardA[i][j] = WATER;
                boardB[i][j] = WATER;
            }
        }
    }

//    printBoard(boardA);
//    cout << endl;
//    printBoard(boardB);
//    cout << endl;

}

