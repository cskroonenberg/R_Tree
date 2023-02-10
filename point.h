#ifndef POINT_H
#define POINT_H

class Point
{
    // TODO: Check if floats are precise enough for 7 decimal coordinate precision
    public:
    float x;
    float y;

    Point(float x, float y) : x(x), y(y) {}

    Point() {
        x = -1;
        y = -1;
    }

    ~Point(){}
};


#endif //POINT_H