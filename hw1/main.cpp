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

using namespace std;

#define MAX_PATH 260

const int rows = 10;
const int cols = 10;
const int NUM_SHIPS = 5;

/* Searches the current working directory for the game configuration files:
 * .sboard -> boardPath
 * .attack-a -> atkPathA
 * .attack-b -> atkPathB
int searchFiles(const string dirPath, string& atkPathA, string& atkPathB, string& boardPath) {
}*/

void initIndividualBoards(string *pString, char **a, char **boardB);

/*string getDirPath() { // uses windows.h
    char buff[MAX_PATH];
    GetModuleFileName(NULL, buff, MAX_PATH); // inserts the executable path into buff
    string tempStr = string(buff);
    return tempStr.substr(0, tempStr.find_last_of("/\\",tempStr.length()-1));
}*/

string getDirPath() {
    char* buff = new char[MAX_PATH];
    buff = _getcwd(buff, MAX_PATH);
    if (!buff) {
        return "!@#"; //signs the string is bad
    }
    string temp(buff);
    delete[] buff;
    return temp;
}

/* Initializes the battle board according to the .sboard file in boardPath.
 * results in a rows*cols board inside passed board arg */
void initBoard(const string boardPath, string* board) {
    ifstream boardFile(boardPath);
    char chars[9] = {' ','B','P','M','D','b','p','m','d'};
    set<char> charSet;
    charSet.insert(chars, chars+9);

    for (int i = 0; i < rows; i++) {
        getline(boardFile, board[i]);
        if (board[i].back() == '\r') board[i].back() = ' '; // handles '\r'
        if (board[i].length() > cols) {                     // ignores extra characters in line
            board[i].erase(cols, board[i].length()-cols);
        }
        else if (board[i].length() < cols) {                // appends extra '_' in case of a short line
            board[i].append(cols-board[i].length(), ' ');
        }
        if (boardFile.eof() && i < rows-1) {                // inserts extra rows of '_' in case of missing lines
            i++;
            for (; i < rows; i++) {
                board[i] = string(cols, ' ');
            }
        }
    }
    for (int i = 0; i < rows; i++) {                        // convert non-valid characters to spaces
        for (int j = 0; j < cols; j++) {
            if (charSet.find(board[i][j]) == charSet.end()) board[i][j] = ' ';
        }
    }
    boardFile.close();
}

/* Checks if the battle board is valid
 * TODO: Eliminate misshapes on the first board iteration
 * */
int checkBoardValidity(string* board) {
    int shipCountA = 0, shipCountB = 0, isShipA = 0, isShipB = 0, adjCheck = 0, ret = 0;
    map<char,int> shipsA = {{'B',1},{'P',2},{'M',3},{'D',4}};
    map<char,int> shipsB = {{'b',1},{'p',2},{'m',3},{'d',4}};
    char shipMapA[4] = {'B','P','M','D'};
    char shipMapB[4] = {'b','p','m','d'};
    bitset<4> errShipsA;                                    // error flags for ship misshapes
    bitset<4> errShipsB;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (board[i][j] != ' ') {
                isShipA = (shipsA[board[i][j]] != 0);       // 1 if its A ship, otherwise 0
                isShipB = (shipsB[board[i][j]] != 0);
                if (isShipA || isShipB) {
                    // Check for misshapes
                    if ((i != 0 && board[i - 1][j] == board[i][j]) +
                        (j != 0 && board[i][j - 1] == board[i][j]) +
                        (i != 0 && j != 0 && board[i - 1][j - 1] == board[i][j]) >= 2) {
                        if (isShipA) {
                            errShipsA[shipsA[board[i][j]] - 1] = 1;
                        }
                        if (isShipB) {
                            errShipsB[shipsB[board[i][j]] - 1] = 1;
                        }
                    }
                    // Check if any adjacent ships exist
                    if (((j != 0) && (board[i][j - 1] != board[i][j]) && (board[i][j - 1] != ' ')) ||
                        ((i != 0) && (board[i - 1][j] != board[i][j]) && (board[i - 1][j] != ' ')) ||
                        ((i != 0 && j != 0) && (board[i - 1][j - 1] != ' '))) {
                        adjCheck = 1;
                    }
                    // check if its new
                    if (!((i != 0 && board[i - 1][j] == board[i][j]) ||
                          (j != 0 && board[i][j - 1] == board[i][j]))) {
                        // check for right ship size
                        int verL = 1, horL = 1;
                        while (j+horL < cols && board[i][j] == board[i][j+horL]) {
                            horL++;
                        }
                        while (i+verL < cols && board[i][j] == board[i+verL][j]) {
                            verL++;
                        }
                        if (verL == shipsB[tolower(board[i][j])] || horL == shipsB[tolower(board[i][j])]) {
                            shipCountA += isShipA;
                            shipCountB += isShipB;
                        } else {
                            if (isShipA) {
                                errShipsA[shipsA[board[i][j]] - 1] = 1;
                            }
                            if (isShipB) {
                                errShipsB[shipsB[board[i][j]] - 1] = 1;
                            }
                        }
                    }
                }
            }
        }
    }

    cout << shipCountA << endl;
    cout << shipCountB << endl;
    // Print possible errors
    for (int i = 0; i < 4; i++) {
        if (errShipsA[i]) {
            cout << "Wrong size or shape for ship " << shipMapA[i] << " for player A" << endl;
            ret = -1;
        }
    }
    for (int i = 0; i < 4; i++) {
        if (errShipsB[i]) {
            cout << "Wrong size or shape for ship " << shipMapB[i] << " for player B" << endl;
            ret = -1;
        }
    }
    if (shipCountA > NUM_SHIPS) {
        cout << "Too many ships for Player A" << endl;
        ret = -1;
    }
    else if (shipCountA < NUM_SHIPS) {
        cout << "Too few ships for Player A" << endl;
        ret = -1;
    }
    if (shipCountB > NUM_SHIPS) {
        cout << "Too many ships for Player B" << endl;
        ret = -1;
    }
    else if (shipCountB < NUM_SHIPS) {
        cout << "Too few ships for Player B" << endl;
        ret = -1;
    }
    if (adjCheck) {
        cout << "Adjacent Ships on Board" << endl;
        ret = -1;
    }
    return ret;
}

/* Fills the attacks vector with the attack moves inside the .attack-a/b file located at atkPath */
void initAttack(const string atkPath, vector<pair<int,int>>& attacks) {
    string line;
    char nextChr;
    int x = -1, y = -1;
    ifstream atkFile(atkPath);

    while(getline(atkFile, line)) {
        if (line.back() == '\r') line.back() = ' ';
        x = -1;
        y = -1;
        stringstream lineStream(line);

        lineStream >> y;                                    //read y coor
        if (y < 1 || y > rows) continue;

        while (lineStream >> nextChr && nextChr == ' ')     //seek comma
            if (lineStream.eof() || nextChr != ',') continue;

        lineStream >> x;                                    //read x coor
        if (x < 1 || x > cols) continue;

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


int main(int argc, char** argv) {
    string dirPath;
    string atkPathA = "../input/clean_movesA.attack-a";
    string atkPathB = "../input/clean_movesB.attack-b";
    string boardPath = "../input/good_board_1.sboard";
	
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


    if (argc == 1) {
        dirPath = getDirPath();
        if (dirPath == "!@#") //error occurred in getDirPath()
        {
            perror("Error");
            return EXIT_FAILURE;
        }
        //searchFiles(argv[1], atkPathA, atkPathB, boardPath);
    }
    else if (argc == 2){
        dirPath = argv[1];
        //searchFiles(dirPath, atkPathA, atkPathB, boardPath);
    }
    else {
        cout << "Error: Too many arguments" << endl;
        return EXIT_FAILURE;
    }
    // setting up the main board
    initBoard(boardPath, board);
    //setting up individual boards
    initIndividualBoards(board,boardA,boardB);
    // setting up attack vectors
    initAttack(atkPathA, attackA);
    initAttack(atkPathB, attackB);


    //now we pass the individual boards, attack vectors and ship lists to the players
    A.setBoard((const char **)boardA, ROW_SIZE, COL_SIZE);
    A.initShipsList();
    A.setMoves(attackA);
    B.setBoard((const char **)boardB, ROW_SIZE, COL_SIZE);
    B.initShipsList();
    B.setMoves(attackB);

    // Let the game begin!!!
    int currentPlayerNum = 0;
    int scores[2] = {0};
    char c;
    Player *pCurrentPlayer = &A;
    char **boardToAttack = boardB;
    std::pair<int,int> currentMove;
    bool gameIsOn = true;
    //string playerName = "A";
    while (gameIsOn)
    {
        currentMove = pCurrentPlayer->attack();
        //cout << playerName << ": (" << currentMove.first << "," << currentMove.second << ")" << endl;

        c = boardToAttack[currentMove.first][currentMove.second];
        if (c == WATER)
        {
            //Miss
        }
        else // Hit
        {



        }




        //switch current player...
        pCurrentPlayer = currentPlayerNum ? &A : &B;
        boardToAttack = currentPlayerNum ? boardB : boardA;
        //playerName = currentPlayerNum ? "A" : "B";
        currentPlayerNum = currentPlayerNum ? 0: 1;

    }




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

    // delete local individual boards
    for (int i = 0; i < COL_SIZE; ++i)
    {
        delete boardA[i];
        delete boardB[i];
    }
    delete boardA;
    delete boardB;

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
