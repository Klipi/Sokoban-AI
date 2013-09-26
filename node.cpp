#include <algorithm>
#include <queue>
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

	vector<Point> usedBoxes;

	for (vector<Point>::iterator j = goals.begin(); j != goals.end(); ++j)
	{
		int minSoFar = 1000;
		Point bestSoFar;

		for (std::vector<Point>::iterator i = state.boxes.begin(); i != state.boxes.end(); ++i)
		{
			if (minSoFar > distance(*i, *j) && find(usedBoxes.begin(), usedBoxes.end(), *i) == usedBoxes.end())
			{
				minSoFar = distance(*i, *j);
				bestSoFar = *i;
			}
		}
		value += minSoFar;
		usedBoxes.push_back(bestSoFar);
	}
	return value;
}

// As below, but only checks for either left-right or up-down. Assumes a box at given Point.
bool Node::isBoxStuck(Point box, bool LRCheck) {

	Point point1, point2;

	if (LRCheck)
	{
		point1 = Point(box.x - 1, box.y);
		point2 = Point(box.x + 1, box.y);

	}
	else
	{
		point1 = Point(box.x, box.y - 1);
		point2 = Point(box.x, box.y + 1);
	}

	Point new_pos[2] = {point1, point2};

	for (size_t i = 0; i < 2; i++)
	{
		// Wall?
		if (clearBoard[(new_pos[i]).y][(new_pos[i]).x] == '#')
			return true;

		// Another box?
		if (find(this->state.boxes.begin(), this->state.boxes.end(), new_pos[i]) != this->state.boxes.end())
			return true;
	}

	return false;
}

// Checks if a box can still be moved. Useful for pruning the search tree. (?)
bool Node::isBoxStuck(Point box) {
	bool stuckLR = this->isBoxStuck(box, true);
	bool stuckUD = this->isBoxStuck(box, false);

	return (stuckLR && stuckUD);
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

bool Node::hasGoalIn(Point place){
	return clearBoard[place.y][place.x] == '.';
}

// For checking if box can be pushed there
bool Node::isFreePoint(Point place){
	return !(hasBoxIn(place) || hasWallIn(place) || clearBoard[place.y][place.x] == '?');
}

vector<Point> Node::getAdjacentBoxGroup(Point box)
{
	if (debug > 8) cerr << "Finding unified group of boxes next to " << (int)box.x << "," << (int)box.y << endl;
	vector<Point> group;
	group.push_back(box);
	queue<Point> frontier = queue<Point>();
	frontier.push(box);

	while (!frontier.empty())
	{
		Point current = frontier.front();
		frontier.pop();

		vector<Point> neighbours = current.getNeighbours();

		for (size_t i = 0; i < neighbours.size(); i++)
		{
			if (find(group.begin(), group.end(), neighbours[i]) == group.end() && this->hasBoxIn(neighbours[i]))
			{
				frontier.push(neighbours[i]);
				group.push_back(neighbours[i]);
			}
		}
	}

	return group;
}

bool Node::identifyDeadGroup(vector<Point> group, Point box){
	vector<Point> stuckGroup;
	if (debug > 8) cerr << "Identifying stuck boxes from a group of " << group.size() << endl;
	for (size_t i = 0; i < group.size(); i++)
	{
		Point current = group[i];
		if (debug > 8) cerr << "Box at " << (int)current.x << "," << (int)current.y << " stuck?" << endl;
		if (this->isBoxStuck(current))
		{
			if (debug > 8) cerr << "Yes" << endl;
			stuckGroup.push_back(current);
		}
	}
	if (find(group.begin(), group.end(), box) == group.end())
		return false;

	if (group.size() == stuckGroup.size())
		return true;
	else
	{
		if (debug > 8) cerr << "Still stuck: " << stuckGroup.size() << endl;
		State newState (this->state.player, stuckGroup);
		Node* newNode = new Node(newState, 'X', this);
		return newNode->identifyDeadGroup(stuckGroup, box);
	}
}
// Returns a node object, where the player has moved one step to dir.
Node* Node::getChild(char dir){
	Point position = Point(state.player.x, state.player.y);
	Point position2;

	if (debug > 7) cerr << "Getting child from point " << (int)state.player.x << "," << (int)state.player.y << " to direction " << dir << endl;

	switch (dir)
	{
		case 'U':
			if (position.y <= 1)
			{
				if (debug > 1) cerr << "Out of bounds ?!?" << endl;
				return new Node(state, 'X', this);
			}
			position = position.up();
			position2 = position.up();
			break;
		case 'R':
			if (position.x >= clearBoard[position.y].size() - 2)
			{
				if (debug > 1) cerr << "Out of bounds ?!?" << endl;
				return new Node(state, 'X', this);
			}
			position = position.right();
			position2 = position.right();
			break;
		case 'D':
			if (position.y >= clearBoard.size() - 2)
			{
				if (debug > 1) cerr << "Out of bounds ?!?" << endl;
				return new Node(state, 'X', this);
			}
			position = position.down();
			position2 = position.down();
			break;
		case 'L':
			if (position.x <= 1)
			{
				if (debug > 1) cerr << "Out of bounds ?!?" << endl;
				return new Node(state, 'X', this);
			}
			position = position.left();
			position2 = position.left();
			break;
	}

	if (debug > 7) cerr << "Moving to " << (int)position.x << "," << (int)position.y << " to direction " << dir << endl;
	if (debug > 7) cerr << "Box will move to " << (int)position2.x << "," << (int)position2.y << " to direction " << dir << endl;
	// Stay in place and use 'X' to denote not moving
	if (hasWallIn(position) || (hasBoxIn(position) && !isFreePoint(position2)))
	{
		if (debug > 7) cerr << "Wall or blocked box ?!?!?" << endl;
		if (debug > 7) cerr << "Wall? " << hasWallIn(position) << endl;
		return new Node(state, 'X', this);
	}

	vector<Point>::iterator pushed_box = find(state.boxes.begin(), state.boxes.end(), position);
	int index = distance(state.boxes.begin(), pushed_box);
	vector<Point> newBoxes = state.boxes;

	if (pushed_box != state.boxes.end())
	{
		if (debug > 7) cerr << "Box pushed to: " << (int)position2.x << "," << (int)position2.y << " to direction " << dir << endl;
		newBoxes[index] = position2;
	}
	sort(newBoxes.begin(), newBoxes.end());

	State newState (position, newBoxes);
	Node* child = new Node(newState, dir, this);

	if (debug > 7) cerr << "Identifying deadlocks" << endl;
	if (!child->hasGoalIn(newBoxes[index]) && child->identifyDeadGroup(child->getAdjacentBoxGroup(newBoxes[index]), newBoxes[index]))
	{
		if (debug > 7) cerr << "Deadlock found with box at " << (int)position2.x << "," << (int)position2.y << endl;
		return new Node(state, 'X', this);
	}
	
	return child;
}
