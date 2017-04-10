#include <iostream>
#include <fstream>
#include <sstream>
#include <direct.h>
#include <bitset>
#include <set>
#include <map>
#include <vector>
#include <stdlib.h>
#include "Player.h"
#include "Bonus.h"

using namespace std;

#define MAX_PATH 1024

const int rows = 10;
const int cols = 10;
const int NUM_SHIPS = 5;

/* Searches the current working directory for the game configuration files:
 * .sboard -> boardPath
 * .attack-a -> atkPathA
 * .attack-b -> atkPathB */
int searchFiles(const string dirPath, string& atkPathA, string& atkPathB, string& boardPath)
{
    string boardSuffix(".sboard");
    string aSuffix(".attack-a");
    string bSuffix(".attack-b");
    string sysDIR("dir \"" + dirPath + "\" /b /a-d > file_names.txt 2>&1");
    const char* sysDIRc = sysDIR.c_str();
    string line;
    int lineSize;
    int pos;
    int ret = 0;

    system(sysDIRc);
    ifstream filenames("file_names.txt");
    while (getline(filenames, line))
    {
        if (line == "File Not Found")
        {
            cout << "Wrong Path " << dirPath << endl;
            filenames.close();
            return -1;
        }
        lineSize = line.length();

        pos = line.rfind(boardSuffix);
        if ((boardPath == "") && (pos != -1) && (pos == lineSize-boardSuffix.length()))
        {
            boardPath = line;
        }

        pos = line.rfind(aSuffix);
        if ((atkPathA == "") && (pos != -1) && (pos == lineSize-aSuffix.length()))
        {
            atkPathA = line;
        }

        pos = line.rfind(bSuffix);
        if ((atkPathB == "") && (pos != -1) && (pos == lineSize-bSuffix.length()))
        {
            atkPathB = line;
        }
    }

    if (boardPath == "")
    {
        cout << "Missing board file (*.sboard) looking in path: " << dirPath << endl;
        ret = -1;
    }
    if (atkPathA == "")
    {
        cout << "Missing attack file for player A (*.attack-a) looking in path: " << dirPath << endl;
        ret = -1;
    }
    if (atkPathB == "")
    {
        cout << "Missing attack file for player B (*.attack-b) looking in path: " << dirPath << endl;
        ret = -1;
    }
    filenames.close();
    return ret;
}

void initIndividualBoards(string *pString, char **a, char **boardB);

string getDirPath()
{
    //I defined MAX_PATH to be 1024 just to get rid of the error
    //Might need TODO a change here
    char* buff = new char[MAX_PATH];
    buff = getcwd(buff, MAX_PATH);
    if (!buff)
    {
        return "!@#"; //signs the string is bad
    }
    string temp(buff);
    delete[] buff;
    return temp;
}

/* Initializes the battle board according to the .sboard file in boardPath.
 * results in a rows*cols board inside passed board arg */
void initBoard(const string boardPath, string* board)
{
    ifstream boardFile(boardPath);
    char chars[9] = {' ','B','P','M','D','b','p','m','d'};
    set<char> charSet;
    charSet.insert(chars, chars+9);

    for (int i = 0; i < rows; i++)
    {
        getline(boardFile, board[i]);
        if (board[i].back() == '\r') board[i].back() = ' '; // handles '\r'
        if (board[i].length() > cols)
        {                     // ignores extra characters in line
            board[i].erase(cols, board[i].length()-cols);
        }
        else if (board[i].length() < cols)
        {                // appends extra '_' in case of a short line
            board[i].append(cols-board[i].length(), ' ');
        }
        if (boardFile.eof() && i < rows-1)
        {                // inserts extra rows of '_' in case of missing lines
            i++;
            for (; i < rows; i++)
            {
                board[i] = string(cols, ' ');
            }
        }
    }
    for (int i = 0; i < rows; i++)
    {                        // convert non-valid characters to spaces
        for (int j = 0; j < cols; j++)
        {
            if (charSet.find(board[i][j]) == charSet.end()) board[i][j] = ' ';
        }
    }
    boardFile.close();
}

int checkShape(string* board, const int size, int i, int j)
{
    int verL = 1, horL = 1;
    // run horizontally, check above and below
    while (j+horL < cols && board[i][j] == board[i][j+horL])
    {
        if ((i+1 < rows && board[i][j] == board[i+1][j+horL]) ||
            (i-1 >= 0 && board[i][j] == board[i-1][j+horL]))
        {
            return -1;
        }
        horL++;
    }
    // run vertically, check right and left
    while (i+verL < rows && board[i][j] == board[i+verL][j])
    {
        if ((j+1 < cols && board[i][j] == board[i+verL][j+1]) ||
            (j-1 >= 0 && board[i][j] == board[i+verL][j-1]))
        {
            return -1;
        }
        verL++;
    }
    // check for misshape in size
    if ((horL > 1 && verL > 1) || (horL != size && verL != size))
    {
        return -1;
    }
    return 1;
}

/* Checks if the battle board is valid */
int checkBoardValidity(string* board)
{
    int shipCountA = 0, shipCountB = 0, isShipA = 0, isShipB = 0, adjCheck = 0, ret = 0;
    map<char,int> shipsA = {{'B',1},{'P',2},{'M',3},{'D',4}};
    map<char,int> shipsB = {{'b',1},{'p',2},{'m',3},{'d',4}};
    char shipMapA[4] = {'B','P','M','D'};
    char shipMapB[4] = {'b','p','m','d'};
    bitset<4> errShipsA;                                    // error flags for ship misshapes
    bitset<4> errShipsB;

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (board[i][j] != ' ')
            {
                isShipA = (shipsA[board[i][j]] != 0);       // 1 if its A ship, otherwise 0
                isShipB = (shipsB[board[i][j]] != 0);
                if (isShipA || isShipB)
                {
                    // check if its new
                    if (!((i != 0 && board[i - 1][j] == board[i][j]) ||
                          (j != 0 && board[i][j - 1] == board[i][j])))
                    {
                        // check for misshape
                        if (checkShape(board, shipsB[tolower(board[i][j])], i, j) < 0)
                        {
                            if (isShipA)
                            {
                                errShipsA[shipsA[board[i][j]] - 1] = 1;
                            }
                            if (isShipB)
                            {
                                errShipsB[shipsB[board[i][j]] - 1] = 1;
                            }
                        }
                        else
                        {
                            shipCountA += isShipA;
                            shipCountB += isShipB;
                        }
                    }
                    // Check if any adjacent ships exist
                    if (((j != 0) && (board[i][j - 1] != board[i][j]) && (board[i][j - 1] != ' ')) ||
                        ((i != 0) && (board[i - 1][j] != board[i][j]) && (board[i - 1][j] != ' ')))
                    {
                        adjCheck = 1;
                    }
                }
            }
        }
    }

    //cout << "Player A has " << shipCountA << " ships" <<endl;
    //cout << "Player B has " << shipCountB << " ships" << endl;
    // Print possible errors
    for (int i = 0; i < 4; i++)
    {
        if (errShipsA[i])
        {
            cout << "Wrong size or shape for ship " << shipMapA[i] << " for player A" << endl;
            ret = -1;
        }
    }
    for (int i = 0; i < 4; i++)
    {
        if (errShipsB[i]) {
            cout << "Wrong size or shape for ship " << shipMapB[i] << " for player B" << endl;
            ret = -1;
        }
    }
    if (shipCountA > NUM_SHIPS)
    {
        cout << "Too many ships for Player A" << endl;
        ret = -1;
    }
    else if (shipCountA < NUM_SHIPS)
    {
        cout << "Too few ships for Player A" << endl;
        ret = -1;
    }
    if (shipCountB > NUM_SHIPS)
    {
        cout << "Too many ships for Player B" << endl;
        ret = -1;
    }
    else if (shipCountB < NUM_SHIPS)
    {
        cout << "Too few ships for Player B" << endl;
        ret = -1;
    }
    if (adjCheck)
    {
        cout << "Adjacent Ships on Board" << endl;
        ret = -1;
    }
    return ret;
}

/* Fills the attacks vector with the attack moves inside the .attack-a/b file located at atkPath */
void initAttack(const string atkPath, vector<pair<int,int>>& attacks)
{
    string line;
    char nextChr;
    int x = -1, y = -1;
    ifstream atkFile(atkPath);

    while(getline(atkFile, line))
    {
        if (line . back() == '\r')
        {
            line . back() = ' ';
        }
        x = -1;
        y = -1;
        stringstream lineStream(line);
        lineStream >> y;                                    //read y coor
        if (y < 1 || y > rows)
        {
            continue;
        }

        while (lineStream >> nextChr && nextChr == ' '){} //seek comma

        if (lineStream . eof() || nextChr != ',')
        {
            continue;
        }

        lineStream >> x;                                    //read x coor
        if (x < 1 || x > cols)
        {
            continue;
        }

        attacks.push_back(make_pair(y,x));
    }
    atkFile.close();
}

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
    POINT p;

    //TODO - add some readl file loading
    string dirPath;
    string atkPathA;
    string atkPathB;
    string boardPath;
    string* board = new string[rows];
    vector<pair<int,int>> attackA;
    vector<pair<int,int>> attackB;
    Player A;
    Player B;
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
    else if (argc == 2)
    {
        dirPath = argv[1];
        if (searchFiles(dirPath, atkPathA, atkPathB, boardPath) < 0)
        {
            return -1;
        }
        boardPath = dirPath + "/" + boardPath;
        atkPathA = dirPath + "/" + atkPathA;
        atkPathB = dirPath + "/" + atkPathB;
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

    //print board
    for (int j = 0; j < COL_SIZE; ++j)
    {
        cout << board[j] << endl;;
    }
    //The game goes on until one of the players has no more ships or both ran out of moves.
    while (pPlayers[0]->hasShips() && pPlayers[1]->hasShips() && (pPlayers[0]->hasMoves() || pPlayers[1]->hasMoves()))
    {
        //Skip if current player is out of moves.
        if (!pPlayers[attackerNum]->hasMoves())
        {
            cout << "Player " << attackerName << " has ran out of moves - SWITCHING PLAYER" << endl;
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
        // todo - delete this (debug) - in this printing we see the ORIGINAL coordinates (without the (-1) offset)
        cout << attackerName << ": (" << (currentMove.first + 1) << "," << (currentMove.second + 1) << ")" << endl;
        cout << "char shot: $" << c << "$" << endl;
        if (c == WATER)
        {
            // Miss
            cout << "It's a miss - no points for you, come back tomorrow - SWITCHING PLAYER" << endl;
        }
        else // Hit xor Sink xor double hit xor hit a sunken ship
        {
            pPlayers[(isupper(c) ? 0 : 1)]->registerHit(currentMove, charToShipType(c), attackResult);

            if (attackResult == AttackResult::Sink)
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
                cout << "It's a hit! The ship has sunk! Yarr!!" << endl;
                cout << "CURRENT SCORE: A-" << scores[0] << ", B-" << scores[1] << endl;
                continue;
            }
            else if (attackResult == AttackResult::Hit)
            {
                //Hit xor self hit
                cout << "It's a" << (!isupper(c)  == attackerNum ? "self " : " ") << " hit!  Yarr!!" << (!isupper(c)  == attackerNum ? "- SWITCHING PLAYER" : "") << endl;
                continue;
            }
            else
            {
                cout << "Double hit or hit a sunken ship - SWITCHING PLAYER" << endl;
            }
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