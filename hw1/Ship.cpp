//
// Created by noamg on 06/04/2017.
//

#include "Ship.h"

Ship::Ship(int size, eShipType type, std::vector<std::pair<int, int>> coordinates)
{
    this->size = size;
    this->type = type;
    this->coordinates = coordinates;
}


void Ship::setType(eShipType type)
{
    this->type = type;
}

void Ship::setCoordinates(std::vector<std::pair<int, int>> coordinates)
{
    this->coordinates = coordinates;
}

void Ship::setSize(int size)
{
    this->size = size;
}

eShipType Ship::getType()
{
    return this->type;
}

std::vector<std::pair<int, int>> Ship::getCoordinates()
{
    return this->coordinates;
}

int Ship::getSize()
{
    return this->size;
}

void Ship::handleHit()
{
    // update only until size == 0 (don't allow negative size)
    if (this->size > 0)
    {
        this->size--;
    }
}

bool Ship::isAlive()
{
    return (this->size > 0);

}

Ship::Ship()
{

}

