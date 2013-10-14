#include <algorithm>
#include <queue>
#include <vector>
#include <unordered_map>
#include "node.hpp"
#include "globals.hpp"
#include "problemSolver.hpp"
#include "goalTest.hpp"

using namespace std;

vector<Node*> pushBoxes(vector<Node*> nodes){
	unordered_map<int, char> directions;
	directions[0] = 'L';
	directions[1] = 'R';
	directions[2] = 'U';
	directions[3] = 'D';
	vector<Node*> pushed;

	for (size_t i = 0; i < nodes.size(); i++)
	{
		Node* tempNode;
		if (debug > 3) cerr << "Push #" << i << " from point " << (int)nodes[i]->state.player.x << "," << (int)nodes[i]->state.player.y << endl;
		vector<Point> neighbours = nodes[i]->state.player.getNeighbours();
		for (size_t j = 0; j < neighbours.size(); j++)
		{
			if (debug > 4) cerr << "Looking at direction " << directions[j] << " and point " << (int)neighbours[j].x << "," << (int)neighbours[j].y << endl;
			if (nodes[i]->hasBoxIn(neighbours[j]))
			{
				if (debug > 5) cerr << "Found box in " << (int)neighbours[j].x << "," << (int)neighbours[j].y << endl;
				if (debug > 6) cerr << "Pushing from point " << (int)nodes[i]->state.player.x << "," << (int)nodes[i]->state.player.y << " to " << directions[j] << endl;
				Node* child = nodes[i]->getChild(directions[j],true);
				if(child != 0)
                    pushed.push_back(child);
			}

		}
	}
	return pushed;
}

int distance(const Point &p1, const Point &p2)
{
	return abs(p1.x - p2.x) + abs(p1.y- p2.y);
}

// Smaller = closer to goal.
int heuristic(const State &state)
{
	int value = 0;

	vector<Point> usedBoxes;

	for (vector<Point>::iterator j = goals.begin(); j != goals.end(); ++j)
	{
		int minSoFar = 1000;
		Point bestSoFar;

		for (std::vector<Point>::const_iterator i = state.boxes.begin(); i != state.boxes.end(); ++i)
		{
			int d = distance(*i, *j);
			if (minSoFar >  d && find(usedBoxes.begin(), usedBoxes.end(), *i) == usedBoxes.end())
			{
				minSoFar = d;
				bestSoFar = *i;
			}
		}
		if(minSoFar>0)
			value += 0.5*distance(*j,state.player);
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
bool Node::hasWallIn(const Point &place){
	//bool ret = clearBoard[place.y][place.x] == '#';
	//if (ret == true)
	//	cerr << "Pushing against wall at " << (int)place.x << "," << (int)place.y << endl;
	return clearBoard[place.y][place.x] == '#';
}

// Checks if there is a box at given Point
bool Node::hasBoxIn(const Point &place){
	return find(state.boxes.begin(), state.boxes.end(), place) != state.boxes.end();
}

bool Node::hasGoalIn(const Point &place){
	return clearBoard[place.y][place.x] == '.';
}

// For checking if box can be pushed there
bool Node::isFreePoint(const Point &place){
	return !(hasBoxIn(place) || hasWallIn(place) || clearBoard[place.y][place.x] == '?');
}

bool Node::isWalkable(const Point &place)
{
	return (!hasBoxIn(place) && !hasWallIn(place));
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
Node* Node::getChild(char dir, bool pushing_allowed = true){
	Point position = Point(state.player.x, state.player.y);
	Point position2;

	if (debug > 7) cerr << "Getting child from point " << (int)state.player.x << "," << (int)state.player.y << " to direction " << dir << endl;

	switch (dir)
	{
		case 'U':
			if (position.y <= 1)
			{
				// if (debug > 1) cerr << "Out of bounds U ?!?" << endl;
				return 0;
			}
			position = position.up();
			position2 = position.up();
			break;
		case 'R':
			if (position.x >= clearBoard[position.y].size() - 2)
			{
				// if (debug > 1) cerr << "Out of bounds R ?!?" << endl;
				return 0;
			}
			position = position.right();
			position2 = position.right();
			break;
		case 'D':
			if (position.y >= clearBoard.size() - 2)
			{
				// if (debug > 1) cerr << "Out of bounds D ?!?" << endl;
				return 0;
			}
			position = position.down();
			position2 = position.down();
			break;
		case 'L':
			if (position.x <= 1)
			{
				// if (debug > 1) cerr << "Out of bounds L ?!?" << endl;
				return 0;
			}
			position = position.left();
			position2 = position.left();
			break;
	}
	
	if(!hasWallIn(position) && !hasBoxIn(position)){
                            State newState (position,state.boxes);
                            return new Node(newState,dir,this);
                            }
	if(!pushing_allowed){
        State newState (position,state.boxes);
        return 0;
        }
 
	if (debug > 7) cerr << "Moving to " << (int)position.x << "," << (int)position.y << " to direction " << dir << endl;
	if (debug > 7) cerr << "Box will move to " << (int)position2.x << "," << (int)position2.y << " to direction " << dir << endl;
	// Stay in place and use 'X' to denote not moving
	if (hasWallIn(position) || (hasBoxIn(position) && !isFreePoint(position2)))
	{
		if (debug > 7) cerr << "Wall or blocked box ?!?!?" << endl;
		if (debug > 7) cerr << "Wall? " << hasWallIn(position) << endl;
		return 0;
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
		return 0;
	}
	
	return child;
}

// Given a node, returns the next nodes to be added to the search queue.
vector<Node*> Node::getNextSteps() {
	vector<Node*> children;

	unordered_map<int, char> directions;
	directions[0] = 'L';
	directions[1] = 'R';
	directions[2] = 'U';
	directions[3] = 'D';

	vector<Point> neighbours = state.player.getNeighbours();

	for (int i = 0; i < directions.size(); i++)
	{
		State newState = State(neighbours[i], state.boxes);
		if (isWalkable(neighbours[i]))
			children.push_back(new Node(newState, directions[i], this));

		if (hasBoxIn(neighbours[i]))
		{
			Point behindBox = neighbours[i].getNeighbours()[i];
			if (isFreePoint(behindBox))
			{
				replace(newState.boxes.begin(), newState.boxes.end(), neighbours[i], behindBox);
				sort(newState.boxes.begin(), newState.boxes.end());
				children.push_back(new Node(newState, directions[i], this));
			}
		}
	}

	return children;
}

// Returns the coordinates from which the given box can be pushed.
vector<Point> Node::getMovableSides(Point box, vector<string> map){
	vector<Point> sides;

	if (!isBoxStuck(box, true)){
		sides.push_back(box.left());
		sides.push_back(box.right());
	}

	if (!isBoxStuck(box, false)){
		sides.push_back(box.up());
		sides.push_back(box.down());
	}

	return sides;
}

// Finds a path to each of goals, where possible.
vector<Node*> Node::findPaths(vector<Point> goals, vector<string> map){
	vector<Node*> paths;
	if(isSearchTarget(goals))
	{
		//cerr << "Start point is goal at " << (int)current->state.player.x << "," <<(int)current->state.player.y << endl;
		paths.push_back(this);
	}
	// else
	// {
	// 	for (size_t i = 0; i < goals.size(); i++)
	// 	{
	// 		cerr << "Goal at " << (int)goals[i].x << "," <<(int)goals[i].y << endl;
	// 	}
	// }

	unordered_map<State, int, StateHash, StateEqual> knownStates;
	knownStates.insert({state, 1});
	priority_queue<Node*, vector<Node*>, NodeCompare> frontier = priority_queue<Node*, vector<Node*>, NodeCompare>();
	frontier.push(this);

	// Same BFS format as in main(). Will exhaust child nodes or find a path to all goals.
	while(!frontier.empty() && goals.size() > 0)
	{
		Node* currentNode = frontier.top();
		frontier.pop();

		//cerr << "Pathfinding frontier has " << frontier.size() << " nodes." << endl;
		knownStates[currentNode->state] = 1;
		vector<Node*> children;
		char directions[4] = {'L','R','U','D'};
		for(size_t i=0; i<4; i++){
                   Node* child = currentNode->getChild(directions[i],false);
                   if(child != 0)
                       children.push_back(child);
                   }
		for(vector<Node*>::iterator i = children.begin();i!=children.end();++i)
		{
			if (knownStates.find((*i)->state) == knownStates.end())
			{
				knownStates[(*i)->state] = 0;
				frontier.push(*i);

				if((*i)->isSearchTarget(goals))
				{
					//cerr << "Found new path to goal at "  << (int)(*i)->state.player.x << "," <<(int)(*i)->state.player.y << endl;
					paths.push_back(*i);
				}
			}
		}
	}

	return paths;
}

// Checks whether one of the targets has been reached.
bool Node::isSearchTarget(vector<Point> &goals){
	for (size_t i = 0; i < goals.size(); i++)
	{
		if (goals[i] == state.player)
		{
			goals.erase(goals.begin() + i);
			return true;
		}
	}

	return false;
}

bool BackNode::isFreePoint(const Point &place){
	return (!hasWallIn(place) && !hasBoxIn(place));
}

vector<Node*> BackNode::getNextSteps()
{
	// cerr << "Backnode, getNextSteps" << endl;
	unordered_map<int, char> directions;
	directions[0] = 'L';
	directions[1] = 'R';
	directions[2] = 'U';
	directions[3] = 'D';

	vector<Node*> ret;
	vector<Point> pos = state.player.getNeighbours();
	int best = 0;
	for( unsigned int i =0;i<pos.size();++i)
	{
		if(isFreePoint(pos[i]))
		{
			int oposite = (i%2==0)?i+1:i-1;
			// cerr << "Has box in " << pos[oposite].toStr() << "? " << hasBoxIn(pos[oposite]) << endl;
			if(hasBoxIn(pos[oposite]))
			{
				// cerr << "Pulling box" << endl;
				State anS = State(pos[i],state.boxes);
				std::replace(anS.boxes.begin(),anS.boxes.end(),pos[oposite],state.player);
				std::sort(anS.boxes.begin(),anS.boxes.end());
				best = ret.size();
				ret.push_back(new BackNode(anS,directions[i],this));
				// continue;
			}
			if(this->parent == NULL || this->parent->state.player != pos[i])
			{
				State nS = State(pos[i],state.boxes);
			    ret.push_back(new BackNode(nS,directions[i],this));
			}
		}
	}
//	std::sort(ret.begin(),ret.end(), NodeCompare());
	if(best>0)
		std::swap( ret[best], ret[0] );
	return ret;
}

bool NoBoxMoveNode::isFreePoint(const Point &place){
	return (!hasWallIn(place) && !hasBoxIn(place));
}

vector<Node*> NoBoxMoveNode::getNextSteps()
{
	unordered_map<int, char> directions;
	directions[0] = 'L';
	directions[1] = 'R';
	directions[2] = 'U';
	directions[3] = 'D';

	vector<Node*> ret;
	vector<Point> pos = state.player.getNeighbours();
	for( unsigned int i =0;i<pos.size();++i)
	{
		if(isFreePoint(pos[i]))
		{
			State nS = State(pos[i],state.boxes);
			if(this->parent == NULL || !(this->parent->state == nS))
                ret.push_back(new NoBoxMoveNode(nS,directions[i],this));
		}
	}
//	std::sort(ret.begin(),ret.end());
	return ret;
}
