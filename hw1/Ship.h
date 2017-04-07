//
// Created by noamg on 06/04/2017.
//

#ifndef HW1_SHIP_H
#define HW1_SHIP_H

#include <vector>
#define BOAT 'B'
#define MISSLE_SHIP 'P'
#define SUBMARINE 'M'
#define DESTROYER 'D'
#define WATER ' '

#define BOAT_SCORE 2
#define MISSLE_SHIP_SCORE 3
#define SUBMARINE_SCORE 7
#define DESTROYER_SCORE 8

enum class eShipType
{
    SHIP_TYPE_B,
    SHIP_TYPE_P,
    SHIP_TYPE_M,
    SHIP_TYPE_D,
    SHIP_TYPE_ERROR
};

class Ship
{
    int size;
    std::vector<std::pair<int,int>> coordinates;
    eShipType type;

public:
    Ship(int size, eShipType type, std::vector<std::pair<int, int>> coordinates);
    Ship();
    void setType(eShipType type);
    void setCoordinates(std::vector<std::pair<int,int>> coordinates);
    void setSize(int size);
    eShipType getType();
    std::vector<std::pair<int,int>> getCoordinates();
    int getSize();
    void handleHit();
    bool isAlive();


};


#endif //HW1_SHIP_H
