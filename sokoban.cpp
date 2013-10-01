#include <iostream>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <queue>
#include <list>
#include <time.h>
#include "sokoban.hpp"
#include "globals.hpp"

using namespace std;

// Returns the coordinates from which the given box can be pushed.
vector<Point> getMovableSides(Point box, vector<string> map, Node *current){
	vector<Point> sides;

	if (!current->isBoxStuck(box, true)){
		sides.push_back(box.left());
		sides.push_back(box.right());
	}

	if (!current->isBoxStuck(box, false)){
		sides.push_back(box.up());
		sides.push_back(box.down());
	}

	return sides;
}

// Checks whether one of the targets has been reached.
bool isSearchTarget(vector<Point> &goals, Node* node){
	for (size_t i = 0; i < goals.size(); i++)
	{
		if (goals[i] == node->state.player)
		{
			goals.erase(goals.begin() + i);
			return true;
		}
	}

	return false;
}

// Finds a path to each of goals, where possible.
vector<Node*> findPaths(vector<Point> goals, vector<string> map, Node *current){
	vector<Node*> paths;
	if(isSearchTarget(goals, current))
	{
		//cerr << "Start point is goal at " << (int)current->state.player.x << "," <<(int)current->state.player.y << endl;
		paths.push_back(current);
	}
	// else
	// {
	// 	for (size_t i = 0; i < goals.size(); i++)
	// 	{
	// 		cerr << "Goal at " << (int)goals[i].x << "," <<(int)goals[i].y << endl;
	// 	}
	// }

	unordered_map<State, int, StateHash, StateEqual> knownStates;
	knownStates.insert({current->state, 1});
	priority_queue<Node*, vector<Node*>, NodeCompare> frontier = priority_queue<Node*, vector<Node*>, NodeCompare>();
	frontier.push(current);

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
                   if(child->direction != 'X')
                       children.push_back(child);
                   }
		for(vector<Node*>::iterator i = children.begin();i!=children.end();++i)
		{
			if (knownStates.find((*i)->state) == knownStates.end())
			{
				knownStates[(*i)->state] = 0;
				frontier.push(*i);

				if(isSearchTarget(goals, *i))
				{
					//cerr << "Found new path to goal at "  << (int)(*i)->state.player.x << "," <<(int)(*i)->state.player.y << endl;
					paths.push_back(*i);
				}
			}
		}
	}

	return paths;
}


void pushBoxes(vector<Node*>& nodes){
	unordered_map<int, char> directions;
	directions[0] = 'L';
	directions[1] = 'R';
	directions[2] = 'U';
	directions[3] = 'D';


	size_t originalSize = nodes.size();

	for (size_t i = 0; i < originalSize; i++)
	{
		Node* tempNode;
		bool first = true;
		if (debug > 3) cerr << "Push #" << i << " from point " << (int)nodes[i]->state.player.x << "," << (int)nodes[i]->state.player.y << endl;
		vector<Point> neighbours = nodes[i]->state.player.getNeighbours();
		for (size_t j = 0; j < neighbours.size(); j++)
		{
			if (debug > 4) cerr << "Looking at direction " << directions[j] << " and point " << (int)neighbours[j].x << "," << (int)neighbours[j].y << endl;
			if (nodes[i]->hasBoxIn(neighbours[j]))
			{

				if (debug > 5) cerr << "Found box in " << (int)neighbours[j].x << "," << (int)neighbours[j].y << endl;
				if (first)
				{
					if (debug > 6) cerr << "Pushing from point " << (int)nodes[i]->state.player.x << "," << (int)nodes[i]->state.player.y << " to " << directions[j] << endl;
					first = false;
					tempNode = nodes[i]->getChild(directions[j],true);
				}
				else
				{
					if (debug > 6) cerr << "Pushing from point " << (int)nodes[i]->state.player.x << "," << (int)nodes[i]->state.player.y << " to " << directions[j] << endl;
					nodes.push_back((nodes[i]->getChild(directions[j],true)));
				}
			}

		}
		nodes[i] = tempNode;
	}
}

// Given a node, returns the next nodes to be added to the search queue.
vector<Node*> getNextSteps(vector<string> map, Node *current) {
	//cerr << "No. of boxes: " << current->state.boxes.size() << endl;
	// Aggregate "interesting" places for the pathfinding algorithm
	vector<Point> nextToBox;
	vector<Node*> foundPaths;
	for (size_t i = 0; i < current->state.boxes.size(); i++)
	{
		Point box = current->state.boxes[i];
		vector<Point> newPoints = getMovableSides(box, map, current);
		
		//if (newPoints.size() == 0 && find(goals.begin(), goals.end(), box) == goals.end() && isBoxInfeasible(box, map, current))
		//{
			//cerr << "Stuck. Box #" << i << ": " << (int)box.x << ", " << (int)box.y << endl;
		//	return foundPaths;
		//}
		nextToBox.insert(nextToBox.end(), newPoints.begin(), newPoints.end());
	}
	if (debug > 2) cerr << "Found " << nextToBox.size() << " interesting points. Finding paths." << endl;
	
	foundPaths = findPaths(nextToBox, map, current);
	if (debug > 2) cerr << "Found " << foundPaths.size() << " paths. Start pushing." << endl;
	
	pushBoxes(foundPaths);
	if (debug > 2) cerr << "Pushing done." << endl;

	return foundPaths;
}

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

bool isGoal(std::vector<Point> goal, State state) {
	// Assume goal and boxes in state is sorted

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

void showSolution(std::vector<std::string> clearBoard, Node* node, string solution){
     showBoard(clearBoard,node->state);
     for(char & c : solution){
                node = node->getChild(c,true);
                showBoard(clearBoard,node->state);
                }
}

//************NOT USED RIGHT NOW, TAKES TOO MUCH TIME************************//
// Finds the lowest reachable position without moving boxes. Used to keep the hash table clean
// (ie. player position is always the same unless something actually critical has happened)
Node* findLowestPlayerPosition(Node* current){

	unordered_map<State, int, StateHash, StateEqual> knownStates;
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
		vector<Node*> children;
		char directions[4] = {'L','R','U','D'};
		for(size_t i=0; i<4; i++){
                   Node* child = newNode->getChild(directions[i],false);
                   if(child->direction != 'X')
                       children.push_back(child);
                   }
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
bool addToHashMap(unordered_map<State, int, StateHash, StateEqual>& knownStates, Node* node, int value){

	if (knownStates.find(node->state) == knownStates.end())
	{
		knownStates.insert({node->state, value});
		return true;
	}
	else
	{
		knownStates[node->state] = value;
		return false;
	}
	
}

int main(int argc, const char **argv) {
	bool verbose = false;
	debug = 0;
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
		else
		{
			std::cerr << "Unknown parameter: '" << argv[i] << "'" << std::endl;
			return -1;
		}
	}
	if (debug > 0) cerr << "Stating debug level " << debug << endl;
	unordered_map<State, int, StateHash, StateEqual> knownStates;
	Node* start = new Node();
	

	goals = std::vector<Point>();


	// Read the board
	std::vector<std::string> board;
	for (std::string line; std::getline(std::cin, line);)
		board.push_back(line);

	parseBoard(board, start, goals, clearBoard);
	sort(goals.begin(), goals.end());
	addToHashMap(knownStates, start, 0);
	int best = heuristic(start->state);
	Node* bestNode = start;

	std::priority_queue<Node*, std::vector<Node*>, NodeCompare> frontier =std::priority_queue<Node*, std::vector<Node*>, NodeCompare>();
	frontier.push(start);
	
	//clock_t start_clock = clock();
    //clock_t t = start_clock;
	//int counter = 0;

	while(!frontier.empty())
	{
        /*if((clock()-t)/(double) CLOCKS_PER_SEC > 1){
                                cerr << counter << endl;
                                counter = 0;
                                t = clock();
                                }
        counter++;*/
		Node* current = frontier.top();
		frontier.pop();
		if (verbose) {
			int value = heuristic(current->state);
			cerr << "Current heuristic value: " << value << endl;
			cerr << "Best heuristic value before: " << best << endl;
			if (value < best )
			{
				best = value;
				bestNode = current;
			}
			std::cerr << getPath(current) << std::endl;
			showBoard(clearBoard, (current)->state);
		}
		if (debug > 5) cerr << "Frontier has " << frontier.size() << " nodes." << endl;
		//knownStates[current->state] = 1;
		if (debug > 1) cerr << "Finding next nodes." << endl;
		std::vector<Node*> children = getNextSteps(clearBoard,current);
		if (debug > 1) cerr << "Search over. Children found:  " << children.size() << endl;

		for(std::vector<Node*>::iterator i = children.begin();i!=children.end();++i)
		{
			if(isGoal(goals,(*i)->state))
			{
				std::string answer = getPath(*i);
				//showSolution(clearBoard,start,answer);
				std::cout << answer << std::endl;
				//cout << (clock()-start_clock)/(double) CLOCKS_PER_SEC;
				return 0;
			}

			if (addToHashMap(knownStates, (*i), 0))
			{
				//std::cerr << "New node found, printing..." << std::endl;
				frontier.push(*i);
			}
		}
	}

	return 0;
}
