#include "Header.h"

Coordinate Rocket::getVelocityVector() const
{
    return velocityVector;
}

void Rocket::updateState()
{
    static constexpr Coordinate startPosition[15] =
    {
        {SCREEN_WIDTH / 4, SCREEN_HEIGHT * 3},
        {(SCREEN_WIDTH * 3) / 4, SCREEN_HEIGHT * 3},
        {SCREEN_WIDTH / 4, -SCREEN_HEIGHT * 2},
        {(SCREEN_WIDTH * 3) / 4, -SCREEN_HEIGHT * 2},
        {SCREEN_WIDTH * 2, SCREEN_HEIGHT / 4},
        {SCREEN_WIDTH * 2, (SCREEN_HEIGHT * 3) / 4},
        {-SCREEN_WIDTH, SCREEN_HEIGHT / 2},
        {-SCREEN_WIDTH, (SCREEN_HEIGHT * 3) / 4},
        {SCREEN_WIDTH / 2, -SCREEN_HEIGHT},
        {SCREEN_WIDTH / 2, SCREEN_HEIGHT * 2},
        {SCREEN_WIDTH * 1, SCREEN_HEIGHT / 3},
        {-SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3},
        {SCREEN_WIDTH * 1, SCREEN_HEIGHT * 1},
        {-SCREEN_WIDTH * 1, SCREEN_HEIGHT * 1},
        {SCREEN_WIDTH * 1, -SCREEN_HEIGHT * 1}
    };


    if (state == STATE_ENABLE_EXPLOSION)
    {
        livingTime.end = CLOCK_NOW();
        if (livingTime.calculateDuration() > ROCKET_EXPLOSION_TIME)
        {
            state = STATE_DISABLE;
            position = startPosition[rand() % 15];
        }
    }
    else
    {
        Coordinate topLeft = {0, 0};
        Coordinate bottomRight = {SCREEN_WIDTH, SCREEN_HEIGHT};

        if (position.y >= bottomRight.y || position.y + size.h <= topLeft.y || position.x >= bottomRight.x || position.x + size.w <= topLeft.x)
        {
            livingTime.end = CLOCK_NOW();
            if (livingTime.calculateDuration() > ROCKET_RESETING_TIME)
            {
                state = STATE_ENABLE_SIGN;
            }
        }
        else
        {
            state = STATE_ENABLE_DEFAULT;
        }
    }
}

bool Rocket::checkCollision_outside(const GameObject *_p_other) const
{
    if (state == STATE_ENABLE_DEFAULT)
    {
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
    return false;
}

void Rocket::findTarget(const Tracker *_p_tracker)
{
    p_previousChasing = p_chasing;
    if (_p_tracker->getState() == Tracker::STATE_DISABLE)
    {
        p_chasing = _p_tracker->getTracking();
    }
    else
    {
        p_chasing = nullptr;
    }
    if (p_chasing == nullptr)
    {
        p_chasing = p_previousChasing;
    }
}

void Rocket::move(const Tracker *_p_tracker)
{
    if (state != STATE_ENABLE_EXPLOSION)
    {
        findTarget(_p_tracker);
        if (p_chasing != nullptr)
        {
            Coordinate target = p_chasing->getPosition() + p_chasing->getSize() / 2;
            Coordinate radiusVector =  target - (position + size / 2);

            angle = radiusVector.calculateAngle() - velocityVector.calculateAngle();
            if (fabs(angle) > M_PI)
            {
                double tmp = (M_PI * 2 - fabs(angle));
                if (angle > 0)
                {
                    angle = -tmp;
                }
                if (angle < 0)
                {
                    angle = tmp;
                }
            }

            angle = velocityVector.calculateAngle() + angle / 2;

            if (angle > M_PI)
            {
                angle -= M_PI * 2;
            }
            if (angle < -M_PI)
            {
                angle += M_PI * 2;
            }

            velocityVector.x = velocityValue * cos(angle);
            velocityVector.y = velocityValue * sin(angle);

            position.x += velocityVector.x;
            position.y += velocityVector.y;

            livingTime.end = CLOCK_NOW();
            if (livingTime.calculateDuration() > ROCKET_LIVING_TIME)
            {
                if (livingTime.calculateDuration() > ROCKET_LIVING_TIME + ROCKET_RESETING_TIME)
                {
                    velocityValue += ROCKET_ACCELERATION;
                }
                else
                {
                    velocityValue = ROCKET_VELOCITY / 2;
                }
            }
        }
    }
}

void Rocket::reset()
{
    p_chasing = nullptr;
    p_previousChasing = nullptr;
    velocityValue = ROCKET_VELOCITY;
    state = STATE_ENABLE_EXPLOSION;
    velocityVector = {0, 0};
    livingTime.begin = CLOCK_NOW();
}

#include "Header.h"

Spaceship *Tracker::getTracking() const
{
    return p_tracking;
}

void Tracker::setTrancking(Spaceship *_p_spaceship)
{
    p_tracking = _p_spaceship;
    state = STATE_DISABLE;
}

void Tracker::setKey(const ShootingControl &_key)
{
    key = _key;
}

bool Tracker::checkCollision_inside(const GameObject *_p_other)
{
    Coordinate topLeft = _p_other->getPosition();
    Coordinate bottomRight = topLeft + _p_other->getSize();

    collision[Side::TOP] = position.y < topLeft.y;
    collision[Side::BOTTOM] = position.y + size.h > bottomRight.y;
    collision[Side::LEFT] = position.x < topLeft.x;
    collision[Side::RIGHT] = position.x + size.w > bottomRight.x;

    return (collision[Side::TOP] || collision[Side::BOTTOM] || collision[Side::LEFT] || collision[Side::RIGHT]);
}

bool Tracker::checkCollision_outside(const GameObject *_p_other)
{
    Coordinate topLeft = _p_other->getPosition();
    Coordinate bottomRight = topLeft + _p_other->getSize();

    bool topIn = (topLeft.y <= position.y) && (position.y <= bottomRight.y);
    bool bottomIn = (topLeft.y <= position.y + size.h) && (position.y + size.h <= bottomRight.y);
    bool leftIn = (topLeft.x <= position.x) && (position.x <= bottomRight.x);
    bool rightIn = (topLeft.x <= position.x + size.w) && (position.x + size.w <= bottomRight.x);

    collision[Side::TOP] = topIn && (leftIn || rightIn) && !bottomIn;
    collision[Side::BOTTOM] = bottomIn && (leftIn || rightIn) && !topIn;
    collision[Side::LEFT] = leftIn && (topIn || bottomIn) && !rightIn;
    collision[Side::RIGHT] = rightIn && (topIn || bottomIn) && !leftIn;

    return (collision[Side::TOP] || collision[Side::BOTTOM] || collision[Side::LEFT] || collision[Side::RIGHT]);
}

void Tracker::findTarget(const std::vector<Spaceship *> &_p_spaceship)
{
    velocityVector = (p_tracking->getVelocityVector() * TRACKER_VELOCITY) / SPACESHIP_VELOCITY;
    if (velocityVector.x != 0 || velocityVector.y != 0)
    {
        p_previousTracking = p_tracking;
        state = STATE_ENABLE_DEFAULT;

        for (auto spaceship : _p_spaceship)
        {
            if (spaceship->getState() != Spaceship::STATE_ENABLE_DEFAULT)
            {
                continue;
            }
            if (p_tracking->getDriver() != spaceship->getDriver())
            {
                Coordinate radiusVector = spaceship->getPosition() - p_tracking->getPosition();
                if ((velocityVector.x * radiusVector.x > 0) && (velocityVector.y * radiusVector.y > 0))
                {
                    p_tracking = spaceship;
                    return;
                }
            }
        }
        p_tracking = nullptr;
    }
}

void Tracker::move(const unsigned char *_p_keyState, const std::vector<Spaceship *> &_p_spaceship)
{
    if (p_tracking == nullptr)
    {
        if (acceleration != 0)
        {
            if (velocityVector.x != 0)
            {
                velocityVector.x -= (velocityVector.x > 0 ? acceleration : -acceleration);
            }
            if (velocityVector.y != 0)
            {
                velocityVector.y -= (velocityVector.y > 0 ? acceleration : -acceleration);
            }
            if (velocityVector.x == 0 && velocityVector.y == 0)
            {
                p_previousTracking = nullptr;
                acceleration = 0;
            }
        }
        position.x += velocityVector.x;
        position.y += velocityVector.y;
    }
    else
    {
        Coordinate target = p_tracking->getPosition() + ((p_tracking->getSize() - this->getSize()) / 2);
        if (state == STATE_ENABLE_DEFAULT)
        {
            Coordinate radiusVector =  target - position;
            double angle = radiusVector.calculateAngle();

            velocityVector.x = TRACKER_VELOCITY * cos(angle);
            velocityVector.y = TRACKER_VELOCITY * sin(angle);

            position.x += velocityVector.x;
            position.y += velocityVector.y;
        }
        else
        {
            position = target;
            if ((_p_keyState[key.p1] && (p_tracking->getDriver() == Player::PLAYER_1))
                || (_p_keyState[key.p2] && (p_tracking->getDriver() == Player::PLAYER_2))
                || (_p_keyState[key.p3] && (p_tracking->getDriver() == Player::PLAYER_3)))
            {

                findTarget(_p_spaceship);
            }
        }
    }
}

void Tracker::handleCollision(const ObjectType &_type, Spaceship *_p_spaceship)
{
    switch (_type)
    {
        case ObjectType::BARRIER:
            if (collision[Side::TOP] || collision[Side::BOTTOM])
            {
                velocityVector.y = -velocityVector.y;
            }
            if (collision[Side::LEFT] || collision[Side::RIGHT])
            {
                velocityVector.x = -velocityVector.x;
            }
            position.x += (collision[Side::LEFT] - collision[Side::RIGHT]) * TRACKER_VELOCITY;
            position.y += (collision[Side::TOP] - collision[Side::BOTTOM]) * TRACKER_VELOCITY;
            acceleration = TRACKER_ACCELERATION;
            break;

        case ObjectType::SPACESHIP:
            if (p_previousTracking == nullptr)
            {
                if (_p_spaceship->getState() == Spaceship::STATE_ENABLE_DEFAULT)
                {
                    p_tracking = _p_spaceship;
                    state = STATE_DISABLE;
                }
            }
            else if (p_previousTracking->getDriver() != _p_spaceship->getDriver())
            {
                if (_p_spaceship->getState() == Spaceship::STATE_ENABLE_DEFAULT)
                {
                    p_tracking = _p_spaceship;
                    state = STATE_DISABLE;
                }
            }
            break;
    }
}

void Tracker::reset(const std::vector<Spaceship *> &_p_spaceship)
{
    double min_distance = 1800;
    Spaceship *_p_nearest_spaceship = nullptr;
    for (auto spaceship : _p_spaceship)
    {
        if (spaceship->getState() != Spaceship::STATE_ENABLE_DEFAULT)
        {
            continue;
        }
        if (p_tracking != nullptr)
        {
            if (p_tracking->getDriver() == spaceship->getDriver())
            {
                return;
            }
        }
        Coordinate radiusVector = spaceship->getPosition() - position;
        double distance = radiusVector.calculateNorm();
        if (distance < min_distance)
        {
            min_distance = distance;
            _p_nearest_spaceship = spaceship;
        }
    }

    p_previousTracking = nullptr;
    p_tracking = _p_nearest_spaceship;
    state = STATE_ENABLE_DEFAULT;
}

