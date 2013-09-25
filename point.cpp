#include "point.hpp"

using namespace std;

Point Point::left(){
	return Point(x-1, y);
}

Point Point::right(){
	return Point(x+1, y);
}

Point Point::up(){
	return Point(x, y+1);
}

Point Point::down(){
	return Point(x, y-1);
}
