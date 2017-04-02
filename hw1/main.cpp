#include <iostream>
#include <fstream>
#include <sstream>
#include <direct.h>
#include <bitset>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <stdlib.h>
#include <stdio.h>
//#include <windows.h>

using namespace std;

const int rows = 10;
const int cols = 10;
const int NUM_SHIPS = 5;

/* Searches the current working directory for the game configuration files:
 * .sboard -> boardPath
 * .attack-a -> atkPathA
 * .attack-b -> atkPathB
int searchFiles(const string dirPath, string& atkPathA, string& atkPathB, string& boardPath) {

}*/

/*string getDirPath() { // uses windows.h
    char buff[MAX_PATH];
    GetModuleFileName(NULL, buff, MAX_PATH); // inserts the executable path into buff
    string tempStr = string(buff);
    return tempStr.substr(0, tempStr.find_last_of("/\\",tempStr.length()-1));
}*/

string getDirPath() {
    char* buff = new char[MAX_PATH];
    buff = getcwd(buff, MAX_PATH);
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

int main(int argc, char** argv) {
    string dirPath;
    string atkPathA = "C:\\Users\\ben\\CLionProjects\\testben\\test.attack-a";
    string atkPathB;
    string boardPath = "C:\\Users\\ben\\CLionProjects\\testben\\test.sboard";
    string* board = new string[rows];
    vector<pair<int,int>> attackA;
    vector<pair<int,int>> attackB;

    if (argc == 1) {
        dirPath = getDirPath();
        //searchFiles(argv[1], atkPathA, atkPathB, boardPath);
    } else {
        dirPath = argv[1];
        //searchFiles(dirPath, atkPathA, atkPathB, boardPath);
    }
    initBoard(boardPath, board);
    initAttack(atkPathA, attackA);

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

    return 0;
}