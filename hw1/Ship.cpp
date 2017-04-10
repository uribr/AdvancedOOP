
#include "Ship.h"

Ship::Ship(int size, eShipType type, std::map<std::pair<int,int>, bool> coordinates)
{
    this->size = size;
    this->type = type;
    this->coordinates = coordinates;
}


void Ship::setType(eShipType type)
{
    this->type = type;
}

void Ship::setCoordinates(std::map<std::pair<int,int>, bool> coordinates)
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

std::map<std::pair<int,int>, bool> Ship::getCoordinates()
{
    return this->coordinates;
}

int Ship::getSize()
{
    return this->size;
}

void Ship::handleHit(std::pair<int,int> coords, AttackResult& res)
{
    // update only until size == 0 (don't allow negative size)
    if (this->coordinates[coords] == true && this->isAlive())
    {
        this->coordinates[coords] = false;
        this->setSize(this->getSize()-1);
        if (isAlive())
        {
            res = AttackResult::Hit;
        }
        else
        {
            res = AttackResult::Sink;
        }

    }
    else
    {
        res = AttackResult::Miss;
    }
}

bool Ship::isAlive()
{
    return (this->size > 0);

}

Ship::Ship()
{

}

