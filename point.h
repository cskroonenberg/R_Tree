#ifndef POINT_H
#define POINT_H

class TreePoint
{
    public:
    float x;
    float y;
    long id;

    TreePoint(float x, float y) : x(x), y(y) {
        id = -1;
    }
    
    TreePoint(float x, float y, long id) : x(x), y(y), id(id) {}

    TreePoint() {
        x = -1;
        y = -1;
    }

    ~TreePoint(){}
};


#endif //POINT_H