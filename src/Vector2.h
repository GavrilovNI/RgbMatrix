#pragma once


template<typename T>
class Vector2
{
public:
    T x;
    T y;
    
    Vector2()
    {
        this->x = 0;
        this->y = 0;
    }

    Vector2(T x, T y)
    {
        this->x = x;
        this->y = y;
    }

    float sqrDistance(Vector2<T> to) const
    {
        float xDist = x - to.x;
        float yDist = y - to.y;
        return xDist * xDist + yDist * yDist;
    }

    T square() const
    {
        return x * y;
    }
};


