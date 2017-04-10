#include "inputUtilities.h"
#include "Player.h"

#define MAX_PATH 1024

using namespace std;

int searchFiles(const string dirPath, string& atkPathA, string& atkPathB, string& boardPath)
{
    string boardSuffix(".sboard");
    string aSuffix(".attack-a");
    string bSuffix(".attack-b");
    string sysDIR("dir 2> errors.txt \"" + dirPath + "\" /b /a-d > file_names.txt");
    const char* sysDIRc = sysDIR.c_str();
    string line;
    int lineSize;
    int pos;
    int ret = 0;

    system(sysDIRc);

    // check for any errors in the directory path
    ifstream errors("errors.txt");
    getline(errors, line);
    if (line != "")
    {
        cout << "Wrong Path " << dirPath << endl;
        errors.close();
        return -1;
    }
    errors.close();

    // parse directory contents
    ifstream filenames("file_names.txt");
    while (getline(filenames, line))
    {
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

void initBoard(const string boardPath, string* board)
{
    ifstream boardFile(boardPath);
    char chars[9] = {' ','B','P','M','D','b','p','m','d'};
    set<char> charSet;
    charSet.insert(chars, chars+9);

    for (int i = 0; i < ROW_SIZE; i++)
    {
        getline(boardFile, board[i]);
        if (board[i].back() == '\r') board[i].back() = ' '; // handles '\r'
        if (board[i].length() > COL_SIZE)
        {                     // ignores extra characters in line
            board[i].erase(COL_SIZE, board[i].length()-COL_SIZE);
        }
        else if (board[i].length() < COL_SIZE)
        {                // appends extra '_' in case of a short line
            board[i].append(COL_SIZE-board[i].length(), ' ');
        }
        if (boardFile.eof() && i < ROW_SIZE-1)
        {                // inserts extra rows of '_' in case of missing lines
            i++;
            for (; i < ROW_SIZE; i++)
            {
                board[i] = string(COL_SIZE, ' ');
            }
        }
    }
    for (int i = 0; i < ROW_SIZE; i++)
    {                        // convert non-valid characters to spaces
        for (int j = 0; j < COL_SIZE; j++)
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
    while (j+horL < COL_SIZE && board[i][j] == board[i][j+horL])
    {
        if ((i+1 < ROW_SIZE && board[i][j] == board[i+1][j+horL]) ||
            (i-1 >= 0 && board[i][j] == board[i-1][j+horL]))
        {
            return -1;
        }
        horL++;
    }
    // run vertically, check right and left
    while (i+verL < ROW_SIZE && board[i][j] == board[i+verL][j])
    {
        if ((j+1 < COL_SIZE && board[i][j] == board[i+verL][j+1]) ||
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

int checkBoardValidity(string* board)
{
    int shipCountA = 0, shipCountB = 0, isShipA = 0, isShipB = 0, adjCheck = 0, ret = 0;
    map<char,int> shipsA = {{'B',1},{'P',2},{'M',3},{'D',4}};
    map<char,int> shipsB = {{'b',1},{'p',2},{'m',3},{'d',4}};
    char shipMapA[4] = {'B','P','M','D'};
    char shipMapB[4] = {'b','p','m','d'};
    bitset<4> errShipsA;                                    // error flags for ship misshapes
    bitset<4> errShipsB;

    for (int i = 0; i < ROW_SIZE; i++)
    {
        for (int j = 0; j < COL_SIZE; j++)
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

    cout << "Player A has " << shipCountA << " ships" <<endl;
    cout << "Player B has " << shipCountB << " ships" << endl;
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
    if (shipCountA > DEFAULT_SHIPS_COUNT)
    {
        cout << "Too many ships for Player A" << endl;
        ret = -1;
    }
    else if (shipCountA < DEFAULT_SHIPS_COUNT)
    {
        cout << "Too few ships for Player A" << endl;
        ret = -1;
    }
    if (shipCountB > DEFAULT_SHIPS_COUNT)
    {
        cout << "Too many ships for Player B" << endl;
        ret = -1;
    }
    else if (shipCountB < DEFAULT_SHIPS_COUNT)
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
//TODO neshama ever heard of parenthesis? I added some parenthesis for readability purposes
        lineStream >> y;                                    //read y coor
        if (y < 1 || y > ROW_SIZE)
        {
            continue;
        }

        while (lineStream >> nextChr && nextChr == ' '){} //seek comma

        if (lineStream . eof() || nextChr != ',')
        {
            continue;
        }

        lineStream >> x;                                    //read x coor
        if (x < 1 || x > COL_SIZE)
        {
            continue;
        }

        attacks.push_back(make_pair(y,x));
    }
    atkFile.close();
}