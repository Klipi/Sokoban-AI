#include <iostream>
#include <unordered_map>
#include <string>
#include <tr1/functional>
#include <algorithm>
#include <queue>
#include <list>
#include "sokoban.hpp"

using namespace std;

// As below, but only checks for either left-right or up-down. Assumes a box at given Point.
bool isBoxStuck(Point box, vector<string> map, Node *current, bool LRCheck) {
	
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
		if (map[(new_pos[i]).y][(new_pos[i]).x] == '#')
			return true;

		// Another box?
		if (std::find(current->state.boxes.begin(), current->state.boxes.end(), new_pos[i]) != current->state.boxes.end())
			return true;
	}

	return false;
}

// Checks if a box can still be moved. Useful for pruning the search tree.
bool isBoxStuck(Point box, vector<string> map, Node *current) {
	return (isBoxStuck(box, map, current, true) && isBoxStuck(box, map, current, false));
}

// Returns the coordinates from which the given box can be pushed.
vector<Point> getMovableSides(Point box, vector<string> map, Node *current){
	vector<Point> sides;

	if (!isBoxStuck(box, map, current, true)){
		sides.push_back(box.left());
		sides.push_back(box.right());
	}

	if (!isBoxStuck(box, map, current, false)){
		sides.push_back(box.up());
		sides.push_back(box.down());
	}

	return sides;
}

std::vector<Node*> getNextSteps(std::vector<std::string> map, Node *current) {
	return possibleSteps(map, current);
}

std::vector<Node*> possibleSteps(std::vector<std::string> map, Node *current) {
	// Find possible steps and put into vector. Johan
	std::vector<Node*> possiblemoves;
	int x = current->state.player.x;
	int y = current->state.player.y;
	int dx[4] = {1, -1, 0, 0};
	int dy[4] = {0, 0, 1, -1};
	char directions[4] = {'R','L','D','U'};

	for(size_t i=0;i<4;i++)
	{
			Node* childstate = new Node();
			Point new_pos;
	        new_pos.x = x+dx[i];
	        new_pos.y = y+dy[i];

		// check if push is possible
		// Which box is pushed (if any)
		std::vector<Point>::iterator pushed_box = std::find(current->state.boxes.begin(), current->state.boxes.end(), new_pos);
		if (pushed_box!=current->state.boxes.end())
		{
			Point new_box_pos;
			new_box_pos.x = x+2*dx[i];
			new_box_pos.y = y+2*dy[i];

			if(
					y+2*dy[i] > 0 && x+2*dx[i] > 0 &&
					(unsigned)(y+2*dy[i]) < map.size() && (unsigned)(x+2*dx[i]) < map[y+2*dy[i]].size() &&
					map[y+2*dy[i]][x+2*dx[i]] != '#' && map[y+2*dy[i]][x+2*dx[i]] != '?' &&
			      	  	(std::find(current->state.boxes.begin(), current->state.boxes.end(), new_box_pos) == current->state.boxes.end()))
			{
				childstate->state.player = new_pos;
				childstate->state.boxes = current->state.boxes;
				childstate->state.boxes[std::distance(current->state.boxes.begin(),pushed_box)] = new_box_pos;
				std::sort(childstate->state.boxes.begin(), childstate->state.boxes.end());
				childstate->direction = directions[i];
				childstate->parent = current;
				possiblemoves.push_back(childstate);
			}
		}

		// check if move is possible
		else if(
				y+dy[i] > 0 && x+dx[i] > 0 &&
				(unsigned)(y+dy[i]) < map.size() && (unsigned)(x+dx[i]) < map[y+dy[i]].size() &&
				map[y+dy[i]][x+dx[i]] != '#')
		{
			childstate->state.player = new_pos;
			childstate->state.boxes = current->state.boxes;
			childstate->direction = directions[i];
			childstate->parent = current;
			possiblemoves.push_back(childstate);
		}

	}
	return possiblemoves;
};


// Parse the board to save the current state, goal points, and a clear version of the board (no player or boxes)
void parseBoard(std::vector<std::string> &map, Node* root, std::vector<Point> &goal, std::vector<string> &clearBoard) {
	// Parse the board and save the current state into currentstate and all possible goal states in the vector
	// Filip
	for (uint8_t i = 0; i<map.size();++i)
	{
		int x;
		int p=0;

		clearBoard.push_back(map[i]);

		while((x=map[i].find('$',p)) < std::string::npos)
		{
			p=x+1;
			root->state.boxes.push_back(Point(x,i));
			clearBoard[i][x] = ' ';
		}

		p=0;
		while((x=map[i].find('.',p))<std::string::npos)
		{
			p=x+1;
			goal.push_back(Point(x,i));
		}

		p=0;
		while((x=map[i].find('*',p))<std::string::npos)
		{
			p=x+1;
			goal.push_back(Point(x,i));
			root->state.boxes.push_back(Point(x,i));
			clearBoard[i][x] = '.';
		}

		if((x=map[i].find('@')) < std::string::npos)
		{
			root->state.player = Point(x,i);
			clearBoard[i][x] = map[i][x] == ' ';
		}
		else if((x=map[i].find('+')) < std::string::npos)
		{
			root->state.player = Point(x,i);
			goal.push_back(Point(x,i));
			clearBoard[i][x] = map[i][x] == '.';
		}
	}

	// Find edgecells

	// Vertical
	// There is never more than like 50 cells in the maps given
	bool edge, LR, UD;
	size_t combo = -1;
	for (size_t x = 1; x < 55; x++)
	{
		edge = false;
		combo = 0;
		for (size_t y = 1; y < clearBoard.size(); y++)
		{
			if (clearBoard[y][x] == '#') {
				if (combo > 0) {
					// Was on a streak but now it hit the wall => fill the holes
					for (size_t i = y-1; i > y-combo-1; i--) {
						clearBoard[i][x] = '?';
					}
				}
				combo = 0;
			}
			else {
				if (combo != -1 && clearBoard[y][x] == ' ') {
				}
				if (
						combo != -1 && clearBoard[y][x] == ' ' && (
						(clearBoard[y][x-1] == '#') ||
						(x+1 < clearBoard[y].size() && clearBoard[y][x+1] == '#')))
				{
					combo++;
				}
				else
				{
					combo = -1;
				}
			}
		}
	}

	// Horisontal and corners
	for (size_t y = 1; y < clearBoard.size(); y++)
	{
		edge = false;
		combo = 0;
		for (size_t x = 1; x < clearBoard[y].size(); x++)
		{
			// Horisontal
			if (clearBoard[y][x] == '#') {
				if (combo > 0) {
					// Was on a streak but now it hit the wall => fill the holes
					for (size_t i = x-1; i > x-combo-1; i--) {
						clearBoard[y][i] = '?';
					}
				}
				combo = 0;
			}
			else {
				if (
						combo != -1 && (clearBoard[y][x] == ' ' || clearBoard[y][x] == '?') && (
						(clearBoard[y-1][x] == '#') ||
						(y+1 < clearBoard.size() && clearBoard[y+1][x] == '#')))
				{
					combo++;
				}
				else
				{
					combo = -1;
				}
			}

			// Corners
			if (clearBoard[y][x] == ' ')
			{
				UD = clearBoard[y-1][x] == '#' || (y+1 < clearBoard.size() && clearBoard[y+1][x] == '#');
				LR = clearBoard[y][x-1] == '#' || (x+1 < clearBoard[y].size() && clearBoard[y][x+1] == '#');

				if (UD && LR)
				{
					clearBoard[y][x] = '?';
				}
			}

		}
	}
};

// Nobody likes global variables but we need it to compare states in the priority queue
std::vector<string> clearBoard;
std::vector<Point> goal = std::vector<Point>();

int distance(Point p1, Point p2)
{
	return abs(p1.x - p2.x) + abs(p1.y- p2.y);
}
// Simple, counts how many boxes on goals
int heuristic(State state)
{
	int value = 0;

	std::sort(goal.begin(),goal.end());
	std::sort(state.boxes.begin(),state.boxes.end());
	for (std::vector<Point>::iterator i = state.boxes.begin(),j = goal.begin(); i != state.boxes.end(); ++i, ++j) {
		value += distance(*i,*j);
	}

	return value;
}


struct NodeCompare
{
	bool operator()(const Node* a, const Node* b) const
	{
		return heuristic(a->state) < heuristic(b->state);
	}
};

bool isGoal(std::vector<Point> goal, State state) {
	// Asume goal and boxes in state is sorted

	for (size_t i = 0; i < state.boxes.size(); i++)
	{
		if (goal[i] != state.boxes[i])
		{
			return false;
		}
	}
	return true;
};

std::string getPath(Node* node) {
	// Returns a string with all steps that led to the state
	std::string path;
	Node *current = node;
	while(current->parent != NULL){
		path.insert(path.begin(),current->direction);
		current = current->parent;
	}
	return path;
};

// Not now lol
State findPathTo(State start, Point goal) {
	/* Find the path for the player to the goal point. Creates the child nodes
	   and returning the last one if reached the goal TODO: Olli? */
	return State();
};

// Sends the board to cerr (used for debugging)
void showBoard(std::vector<std::string> clearBoard, State state) {
	// PLayer position
	clearBoard[state.player.y][state.player.x] = clearBoard[state.player.y][state.player.x] == '.' ? '+' : '@';

	for(std::vector<Point>::iterator i = state.boxes.begin(); i != state.boxes.end(); ++i)
	{
		clearBoard[i->y][i->x] = clearBoard[i->y][i->x] == '.' ? '*' : '$';
	}

	// Send it to cerr
	for (std::vector<string>::iterator row = clearBoard.begin(); row != clearBoard.end(); row++) {
		std::cerr << *row << std::endl;
	}
	std::cerr << std::endl;
};

int main(int argc, const char **argv) {
	bool verbose = false;
	for (int i = 1; i < argc; ++i)
	{
		std::string param(argv[i]);
		if (param == "verbose" || param == "v")
			verbose = true;
		else
		{
			std::cerr << "Unknown parameter: '" << argv[i] << "'" << std::endl;
			return -1;
		}
	}

	unordered_map<State, int, StateHash, StateEqual> knownStates;
	Node* start = new Node();

	// Read the board
	std::vector<std::string> board;
	for (std::string line; std::getline(std::cin, line);)
		board.push_back(line);

	parseBoard(board, start, goal, clearBoard);

	std::priority_queue<Node*, std::vector<Node*>, NodeCompare> frontier =std::priority_queue<Node*, std::vector<Node*>, NodeCompare>();
	frontier.push(start);

	while(!frontier.empty())
	{
		Node* current = frontier.top();
		knownStates.insert({current->state, 1});
		frontier.pop();
		std::vector<Node*> children = getNextSteps(clearBoard,current);
		for(std::vector<Node*>::iterator i = children.begin();i!=children.end();++i)
		{
			if(isGoal(goal,(*i)->state))
			{
				std::string answer = getPath(*i);
				std::cout << answer << std::endl;
				return 0;
			}

			if (knownStates.find((*i)->state) == knownStates.end())
			{
				// std::cerr << " that was cool" << std::endl;
				if (verbose) {
					std::cerr << getPath(*i) << std::endl;
					showBoard(clearBoard, (*i)->state);
				}
				frontier.push(*i);
			}
		}
	}

	return 0;
}
