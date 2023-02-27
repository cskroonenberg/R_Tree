#ifndef POINT_H
#define POINT_H

class TreePoint
{
    public:
    double x;
    double y;
    long id;

    TreePoint(double x, double y) : x(x), y(y) {
        id = -1;
    }
    
    TreePoint(double x, double y, long id) : x(x), y(y), id(id) {}

    TreePoint() {
        x = -1;
        y = -1;
    }

    ~TreePoint(){}
};


#endif //POINT_H