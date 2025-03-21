#include "Frozen.h"

void Frozen::updateState() {
    position = {800, 800};
}


bool Frozen::checkCollision_outside(const GameObject *_p_other){
    Coordinate topLeft = _p_other->getPosition();
    Coordinate bottomRight = topLeft + _p_other->getSize();

    if (position.y >= bottomRight.y)
    {
        return false;
    }
    if (position.y + size.h <= topLeft.y)
    {
        return false;
    }
    if (position.x >= bottomRight.x)
    {
        return false;
    }
    if (position.x + size.w <= topLeft.x)
    {
        return false;
    }
    return true;
}
void Frozen::reset()
{
    existingTime.begin = CLOCK_NOW();
}


