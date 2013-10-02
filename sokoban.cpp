#include <iostream>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <queue>
#include <list>
#include "sokoban.hpp"
#include "globals.hpp"
#include <time.h>

using namespace std;


// Parse the board to save the current state, goal points, and a clear version of the board (no player or boxes)
void parseBoard(std::vector<std::string> &map, Node* root, std::vector<Point> &goal, std::vector<string> &clearBoard) {
	// Parse the board and save the current state into currentstate and all possible goal states in the vector
	// Filip
	for (uint8_t i = 0; i<map.size();++i)
	{
		unsigned int x;
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
			map[i][x] = ' ';
			clearBoard[i][x] = ' ';
		}
		else if((x=map[i].find('+')) < std::string::npos)
		{
			root->state.player = Point(x,i);
			goal.push_back(Point(x,i));
			map[i][x] = '.';
			clearBoard[i][x] = '.';
		}
	}

	// Find edgecells

	// Vertical
	// There is never more than like 50 cells in the maps given
	bool LR, UD;
	size_t combo = -1;
	for (size_t x = 1; x < 55; x++)
	{
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

bool isGoal(const std::vector<Point>& goal, State& state) {
	// Assume goal and boxes in state is sorted
	std::sort(state.boxes.begin(),state.boxes.end());
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
		if (current->direction != 'X')
			path.insert(path.begin(),current->direction);
		current = current->parent;
	}
	return path;
};

// Not now lol
State findPathTo(const State start, const Point goal) {
	/* Find the path for the player to the goal point. Creates the child nodes
	   and returning the last one if reached the goal TODO: Olli? */
	return State();
};

// Sends the board to cerr (used for debugging)
void showBoard(std::vector<std::string> &clearBoard, const State& state) {
	// PLayer position
	clearBoard[state.player.y][state.player.x] = clearBoard[state.player.y][state.player.x] == '.' ? '+' : '@';

	for(std::vector<Point>::const_iterator i = state.boxes.begin(); i != state.boxes.end(); ++i)
	{
		clearBoard[i->y][i->x] = clearBoard[i->y][i->x] == '.' ? '*' : '$';
	}

	// Send it to cerr
	for (std::vector<string>::iterator row = clearBoard.begin(); row != clearBoard.end(); row++) {
		std::cerr << *row << std::endl;
	}
	std::cerr << std::endl;
};

// Finds the lowest reachable position without moving boxes. Used to keep the hash table clean
// (ie. player position is always the same unless something actually critical has happened)
Node* findLowestPlayerPosition(Node* current){

	unordered_map<State, int, StateHash> knownStates;
	Node* start = current;
	Node* lowest = start;
	knownStates.insert({start->state, 0});
	//cerr << "Finding the common position, start: " << (int)lowest->state.player.x << "," << (int)lowest->state.player.y << endl;
	std::priority_queue<Node*, std::vector<Node*>, NodeCompare> frontier =std::priority_queue<Node*, std::vector<Node*>, NodeCompare>();
	frontier.push(start);

	while(!frontier.empty())
	{
		Node* newNode = frontier.top();
		frontier.pop();
		//cerr << "Frontier has " << frontier.size() << " nodes." << endl;

		knownStates[current->state] = 1;
		//cerr << "Finding next nodes." << endl;
		std::vector<Node*> children = newNode->possibleSteps(clearBoard, true);
		//cerr << "Search over. Children found:  " << children.size() << endl;
		//cerr << "Printing children" << endl;
		for(std::vector<Node*>::iterator i = children.begin();i!=children.end();++i)
		{
			if (knownStates.find((*i)->state) == knownStates.end())
			{
				knownStates.insert({(*i)->state, 0});
				frontier.push(*i);

				if(lowest->state.player > (*i)->state.player)
				{
					lowest = *i;
					//cerr << "Found better: " << (int)lowest->state.player.x << "," << (int)lowest->state.player.y << endl;
				}
			}
		}

	}

	return lowest;
}

// Returns true if a new element was added
bool addToHashMap(unordered_map<State, int, StateHash>& knownStates, Node* node, int value){
	Node* newNode = findLowestPlayerPosition(node);

	if (knownStates.find(newNode->state) == knownStates.end())
	{
		knownStates.insert({newNode->state, value});
		return true;
	}
	else
	{
		knownStates[newNode->state] = value;
		return false;
	}
	
}

int main(int argc, const char **argv) {
	bool verbose = false;
	debug = 0;
	bool time = true;
	for (int i = 1; i < argc; ++i)
	{
		std::string param(argv[i]);
		if (param == "verbose" || param == "v")
		{
			verbose = true;
		}
		else if (param == "debug" || param == "d")
		{
			if (i + 1 < argc)
				debug = (int)*argv[++i] - '0';
			else
				std::cerr << "Usage: '" << argv[i] << " <int>' " << std::endl;
		}
		else if(param == "time" || param == "t")
			time=true;
		else
		{
			std::cerr << "Unknown parameter: '" << argv[i] << "'" << std::endl;
			return -1;
		}
	}
	//cerr << debug << endl;
	unordered_map<State, int, StateHash> knownStates;
	Node* start = new Node();
	
	goals = std::vector<Point>();

	// Read the board
	std::vector<std::string> board;
	for (std::string line; std::getline(std::cin, line);)
		board.push_back(line);

	parseBoard(board, start, goals, clearBoard);
	sort(goals.begin(), goals.end());
	clock_t begin = clock();
	addToHashMap(knownStates, start, 0);

	std::priority_queue<Node*, std::vector<Node*>, NodeCompare> frontier =std::priority_queue<Node*, std::vector<Node*>, NodeCompare>();
	frontier.push(start);

	while(!frontier.empty())
	{
		Node* current = frontier.top();
		frontier.pop();
		if (verbose) {
			std::cerr << getPath(current) << std::endl;
			showBoard(clearBoard, (current)->state);
		}
		if (debug > 5) cerr << "Frontier has " << frontier.size() << " nodes." << endl;

		//knownStates[current->state] = 1;
		if (debug > 1) cerr << "Finding next nodes." << endl;
		std::vector<Node*> children = current->getNextSteps(clearBoard);
		if (debug > 1) cerr << "Search over. Children found:  " << children.size() << endl;
		//cerr << "Printing children" << endl;
		for(std::vector<Node*>::iterator i = children.begin();i!=children.end();++i)
		{
			if(isGoal(goals,(*i)->state))
			{
				std::string answer = getPath(*i);
				std::cout << answer << std::endl;
				if(time)
				{
					clock_t t = clock() - begin;
					printf ("%f seconds.\n",((float)t)/CLOCKS_PER_SEC);
				}
				return 0;
			}

			if (addToHashMap(knownStates, (*i), 0))
			{
				//std::cerr << "New node found, printing..." << std::endl;
				frontier.push(*i);
			}
		}

	}

	if(time)
	{
		clock_t t = clock() - begin;
		printf ("%f seconds.\n",((float)t)/CLOCKS_PER_SEC);
	}
	return 0;
}
