#ifndef POINT_H
#define POINT_H

#include<cstdint>

class Point {
    public:
        uint8_t x, y;

        bool operator < (const Point& other) const
        {
        	if (y == other.y)
        		return (x < other.x);
            else
            	return (y < other.y);
        }
        bool operator == (const Point& other) const
        {
        	return x == other.x && y == other.y;
        }
        bool operator != (const Point& other) const
        {
        	return !(*this == other);
        }

        Point():x(0),y(0){}

        Point(uint8_t px, uint8_t py): x(px), y(py){}

        Point left();
        Point right();
        Point up();
        Point down();
};

#endif