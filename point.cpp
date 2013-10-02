#include "point.hpp"

using namespace std;

Point Point::left() const{
	return Point(x-1, y);
}

Point Point::right() const{
	return Point(x+1, y);
}

Point Point::up() const{
	return Point(x, y-1);
}

Point Point::down() const{
	return Point(x, y+1);
}

// Gets the four neighbours for a point
vector<Point> Point::getNeighbours() const{
	vector<Point> neighbours;
	neighbours.push_back(this->left());
	neighbours.push_back(this->right());
	neighbours.push_back(this->up());
	neighbours.push_back(this->down());
	return neighbours;
}
