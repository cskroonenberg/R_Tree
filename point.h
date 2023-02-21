#ifndef POINT_H
#define POINT_H

class Point
{
    // TODO: Check if floats are precise enough for 7 decimal coordinate precision
    public:
    double x;
    double y;
    long id;

    Point(double x, double y) : x(x), y(y) {
        id = -1;
    }
    
    Point(double x, double y, long id) : x(x), y(y), id(id) {}

    Point() {
        x = -1;
        y = -1;
    }

    ~Point(){}
};


#endif //POINT_H