#include <algorithm>
#include "node.hpp"
#include "globals.hpp"

using namespace std;

int distance(Point p1, Point p2)
{
	return abs(p1.x - p2.x) + abs(p1.y- p2.y);
}

// Simple, counts how many boxes on goals
int heuristic(State state)
{
	int value = 0;

	for (std::vector<Point>::iterator i = state.boxes.begin(),j = goals.begin(); i != state.boxes.end(); ++i, ++j) {
		value += distance(*i,*j);
	}

	return value;
}

// Returns true if there is a wall at given Point
bool Node::hasWallIn(Point place){
	return clearBoard[place.y][place.x] == '#';
}

// Checks if there is a box at given Point
bool Node::hasBoxIn(Point place){
	return find(state.boxes.begin(), state.boxes.end(), place) != state.boxes.end();
}

// For checking if box can be pushed there
bool Node::isFreePoint(Point place){
	return !(hasBoxIn(place) || hasWallIn(place));
}

// Returns a node object, where the player has moved one step to dir.
Node* Node::getChild(char dir){
	Point position = state.player;

	// Check for edges in case of out-of-range errors
	if (position.x <= 1 || position.y <= 1 ||
			 position.y >= clearBoard.size() - 2 ||
			 position.x >= clearBoard[0].size() - 2)
		{
			return new Node(state, 'X', this);
		}

	Point position2;
	switch (direction)
	{
		case 'U':
			position = position.up();
			position2 = position.up();
			break;
		case 'R':
			position = position.right();
			position2 = position.right();
			break;
		case 'D':
			position = position.down();
			position2 = position.down();
			break;
		case 'L':
			position = position.left();
			position2 = position.left();
			break;
	}

	// Stay in place and use 'X' to denote not moving
	if (hasWallIn(position) || (hasBoxIn(position) && !isFreePoint(position2)))
		return new Node(state, 'X', this);

	vector<Point>::iterator pushed_box = find(state.boxes.begin(), state.boxes.end(), position);

	vector<Point> newBoxes = state.boxes;
	if (pushed_box != state.boxes.end())
		newBoxes[distance(state.boxes.begin(), pushed_box)] = position2;

	
	State newState (position, newBoxes);
	Node* child = new Node(newState, dir, this);

	return child;
}