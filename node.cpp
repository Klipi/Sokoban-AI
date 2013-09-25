#include <algorithm>
#include "node.hpp"
#include "globals.hpp"

using namespace std;

int distance(Point p1, Point p2)
{
	return abs(p1.x - p2.x) + abs(p1.y- p2.y);
}

// Smaller = closer to goal.
int heuristic(State state)
{
	int value = 0;

	for (std::vector<Point>::iterator i = state.boxes.begin(); i != state.boxes.end(); ++i) {
		//if (find(goals.begin(), goals.end(), *i) != goals.end())
		//{
		//	value -= 20;
		//	continue;
		//}
		int minSoFar = 1000;
		for (vector<Point>::iterator j = goals.begin(); j != goals.end(); ++j)
		{
			if (find(state.boxes.begin(), state.boxes.end(), *j) == state.boxes.end())
			{
				minSoFar = min(minSoFar, distance(*i, *j));
			}
		}
		value += minSoFar;
	}

	return value;
}

// Returns true if there is a wall at given Point
bool Node::hasWallIn(Point place){
	//bool ret = clearBoard[place.y][place.x] == '#';
	//if (ret == true)
	//	cerr << "Pushing against wall at " << (int)place.x << "," << (int)place.y << endl;
	return clearBoard[place.y][place.x] == '#';
}

// Checks if there is a box at given Point
bool Node::hasBoxIn(Point place){
	return find(state.boxes.begin(), state.boxes.end(), place) != state.boxes.end();
}

// For checking if box can be pushed there
bool Node::isFreePoint(Point place){
	return !(hasBoxIn(place) || hasWallIn(place) || clearBoard[place.y][place.x] == '?');
}


bool Node::identifyDeadBox(Node* node, Point box){
	return false;
}
// Returns a node object, where the player has moved one step to dir.
Node* Node::getChild(char dir){
	Point position = state.player;
	Point position2;

	//cerr << "Getting child from point " << (int)state.player.x << "," << (int)state.player.y << " to direction " << dir << endl;

	switch (dir)
	{
		case 'U':
			if (position.y <= 1)
			{
				//cerr << "Out of bounds ?!?" << endl;
				return new Node(state, 'X', this);
			}
			position = position.up();
			position2 = position.up();
			break;
		case 'R':
			if (position.x >= clearBoard[position.y].size() - 2)
			{
				//cerr << "Out of bounds ?!?" << endl;
				return new Node(state, 'X', this);
			}
			position = position.right();
			position2 = position.right();
			break;
		case 'D':
			if (position.y >= clearBoard.size() - 2)
			{
				//cerr << "Out of bounds ?!?" << endl;
				return new Node(state, 'X', this);
			}
			position = position.down();
			position2 = position.down();
			break;
		case 'L':
			if (position.x <= 1)
			{
				//cerr << "Out of bounds ?!?" << endl;
				return new Node(state, 'X', this);
			}
			position = position.left();
			position2 = position.left();
			break;
	}

	//cerr << "Moving to " << (int)position.x << "," << (int)position.y << " to direction " << dir << endl;
	//cerr << "Box will move to " << (int)position2.x << "," << (int)position2.y << " to direction " << dir << endl;
	// Stay in place and use 'X' to denote not moving
	if (hasWallIn(position) || (hasBoxIn(position) && !isFreePoint(position2)))
	{
		//cerr << "Wall or blocked box ?!?!?" << endl;
		//cerr << "Wall? " << hasWallIn(position) << endl;
		return new Node(state, 'X', this);
	}

	vector<Point>::iterator pushed_box = find(state.boxes.begin(), state.boxes.end(), position);

	vector<Point> newBoxes = state.boxes;
	if (pushed_box != state.boxes.end())
		newBoxes[distance(state.boxes.begin(), pushed_box)] = position2;



	State newState (position, newBoxes);
	Node* child = new Node(newState, dir, this);

	if (identifyDeadBox(child, newBoxes[distance(state.boxes.begin(), pushed_box)]))
	{
		return new Node(state, 'X', this);
	}

	return child;
}