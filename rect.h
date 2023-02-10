#ifndef RECT_H
#define RECT_H

#include "point.h"

class Rect
{
    private:
    Point low;  // Lower left point
    Point high; // Upper right point

    public:
    Rect(Point high, Point low) : low(low), high(high){}

    Rect(){}

    ~Rect(){
    }

    void set(float high_x, float high_y, float low_x, float low_y) {
        high = Point(high_x, high_y);
        low = Point(low_x, low_y);
    }

    Point getLow(){
        return low;
    }

    Point getHigh(){
        return high;
    }

    bool contains(Point p){
        return p.x >= low.x && p.x <= high.x && p.y >= low.y && p.y <= high.y;
    }

    bool contains(Rect r){
        return contains(r.getLow()) && contains(r.getHigh());
    }

    float area() {
        return (high.x - low.x) * (high.y - low.y);
    }

    // Return the area of the Rect if Point p were to be inserted
    float what_if_area(Point p) {
        Point what_if_high(high.x, high.y);
        Point what_if_low(low.x, low.y);

        if(p.x > what_if_high.x) {
            what_if_high.x = p.x;
        } else if (p.x < what_if_low.x) {
            what_if_low.x = p.x;
        }

        if(p.y > what_if_high.y) {
            what_if_high.y = p.y;
        } else if (p.y < what_if_low.y) {
            what_if_low.y = p.y;
        }

        return (what_if_high.x - what_if_low.x) * (what_if_high.y - what_if_low.y);
    }

    float what_if_area_rect(Rect r) {
        Point what_if_high(high.x, high.y);
        Point what_if_low(low.x,low.y);

        if(r.getHigh().x > what_if_high.x) {
            what_if_high.x = r.getHigh().x;
        } else if (r.getLow().x < what_if_low.x) {
            what_if_low.x = r.getLow().x;
        }

        if(r.getHigh().y > what_if_high.y) {
            what_if_high.y = r.getHigh().y;
        } else if (r.getLow().y < what_if_low.y) {
            what_if_low.y = r.getLow().y;
        }

        return (what_if_high.x - what_if_low.x) * (what_if_high.y - what_if_low.y);
    }

    // Return the increase in area if a Point p were to be added to the Rect
    float enlargement(Point p) {
        return what_if_area(p) - area();
    }

    // Return the increase in area if a Rect r were to be added to the Rect
    float enlargementRect(Rect r) {
        return what_if_area_rect(r) - area();
    }

    // Update Rect bounds with new contained Point, p
    void updatePoint(Point p) {
        if(p.x > high.x) {
            high.x = p.x;
        } else if (p.x < low.x) {
            low.x = p.x;
        }

        if(p.y > high.y) {
            high.y = p.y;
        } else if (p.y < low.y) {
            low.y = p.y;
        }
    }

    void updateRect(Rect r) {
        updatePoint(r.getLow());
        updatePoint(r.getHigh());
    }
};

#endif //RECT_H