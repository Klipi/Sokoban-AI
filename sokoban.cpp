#include <iostream>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <queue>
#include <list>
#include <time.h>
#include "sokoban.hpp"
#include "globals.hpp"
#include "problemSolver.hpp"

using namespace std;


// Parse the board to save the current state, goal points, and a clear version of the board (no player or boxes)
void parseBoard(std::vector<std::string> &map, Node* root, std::vector<Point> &goal, std::vector<string> &clearBoard, bool back, Point &initialPlayerPos) {
	// Parse the board and save the current state into currentstate and all possible goal states in the vector
	// Filip
	bool placed = false;
	for (uint8_t i = 0; i<map.size();++i)
	{
		int x;
		int p=0;

		clearBoard.push_back(map[i]);

		while((x=map[i].find('$',p)) < std::string::npos)
		{
			p=x+1;
			if(back)
			{
				goal.push_back(Point(x,i));
				clearBoard[i][x] = '.';
			}
			else
			{
				root->state.boxes.push_back(Point(x,i));
				clearBoard[i][x] = ' ';
			}
		}

		p=0;
		while((x=map[i].find('.',p))<std::string::npos)
		{
			p=x+1;
			if(back)
			{
				root->state.boxes.push_back(Point(x,i));
				clearBoard[i][x] = ' ';
			}
			else
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
			if(!back)
			{
				//cout << "x: " << x << ", y: " << int(i) << endl;
				root->state.player = Point(x,i);
				placed = true;
			}
			else
			{
				initialPlayerPos.x=x;
				initialPlayerPos.y=i;
			}
			map[i][x] = ' ';
			clearBoard[i][x] = ' ';
		}
		else if((x=map[i].find('+')) < std::string::npos)
		{
			if(back)
			{
				initialPlayerPos.x=x;
				initialPlayerPos.y=i;
				Point pos(x,i);
				root->state.boxes.push_back(pos);
				map[i][x] = ' ';
				clearBoard[i][x] = ' ';
				vector<Point> posi = pos.getNeighbours();
				for(int i=0;i<posi.size();++i)
				{
					if(root->isFreePoint(posi[i]))
					{
						root->state.player = posi[i];
						placed=true;
						break;
					}
				}
			}
			else
			{
				root->state.player = Point(x,i);
				goal.push_back(Point(x,i));
				map[i][x] = '.';
				clearBoard[i][x] = '.';
				placed = true;
			}
		}
	}
	if(!placed)
	{
		for(int j=0;j<root->state.boxes.size();++j)
		{
			vector<Point> pos = root->state.boxes[j].getNeighbours();
			for(int i=0;i<pos.size();++i)
			{
				if(root->isFreePoint(pos[i]))
				{
					State start(pos[i],root->state.boxes);
					ProblemSolver findPathToGoals(new MovePlayerToBoxTest(), goal, new NoBoxMoveNode(start,' ',0));
					std::string path = findPathToGoals.findSolution();
					if(path!="")
					{
						root->state.player = pos[i];
						placed=true;
						break;
					}
				}
			}
			if(placed)
				break;
		}
	}
    
    if(!back)
    {
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
    }
};

std::string getPath(Node* node) {
	// Returns a string with all steps that led to the state
	std::string path;
	Node *current = node;
	while(current->parent != NULL){
        path=current->direction+path;
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
                   if(child != 0)
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

// Returns true if a new value was added. This is the overloaded version, which returns the new value.
bool addToHashMap(unordered_map<State, int, StateHash, StateEqual>& knownStates, Node* node, int value, int &newValue, bool combine = false){
	Node* newNode;
	if (combine)
		newNode = findLowestPlayerPosition(node);
	else
		newNode = node;

	if (knownStates.find(newNode->state) == knownStates.end())
	{
		knownStates.insert({newNode->state, value});
		newValue = value;
		return true;
	}
	else
	{
		newValue = knownStates[newNode->state];
		if (newValue != value)
		{
			//cerr << "Old value: " << newValue << ", value to be added " << value << endl;
			knownStates[newNode->state] += value;
			newValue += value;
		}
		
		return false;
	}
	
}

// Returns true if a new value was added
bool addToHashMap(unordered_map<State, int, StateHash, StateEqual>& knownStates, Node* node, int value){
	int dummy = 0;
	return addToHashMap(knownStates, node, value, dummy, false);
}

std::string reversePath(std::string& path)
{
	std::vector<char> tmp;
	for(int i=path.size()-1;i>=0;--i)
	{
		switch(path[i])
		{
		case 'L':
			tmp.push_back('R');
			break;
		case 'R':
			tmp.push_back('L');
			break;
		case 'U':
			tmp.push_back('D');
			break;
		case 'D':
			tmp.push_back('U');
			break;
		default:
			tmp.push_back(' ');
		}
	}
	return std::string(tmp.begin(),tmp.end());
}

// Returns true if goal expansion finds an ordinary node or vice versa
bool expandFrontier(priority_queue<Node*, vector<Node*>, NodeCompare> &frontier, unordered_map<State, int, StateHash, StateEqual> &knownStates, int hashMapID, bool verbose, bool back)
{
	Node* current = frontier.top();
	frontier.pop();
	if (verbose) {
		int value = heuristic(current->state);
		cerr << "Current heuristic value: " << value << endl;
		// cerr << "Best heuristic value before: " << best << endl;
		// if (value < best )
		// {
		// 	best = value;
		// 	bestNode = current;
		// }
		std::cerr << "Path: " << getPath(current) << std::endl;
		showBoard(clearBoard, (current)->state);
	}
	if (debug > 5) cerr << "Frontier has " << frontier.size() << " nodes." << endl;
	//knownStates[current->state] = 1;
	if (debug > 1) cerr << "Finding next nodes." << endl;
	std::vector<Node*> children = current->getNextSteps();
	if (debug > 1) cerr << "Search over. Children found:  " << children.size() << endl;

	for(std::vector<Node*>::iterator i = children.begin();i!=children.end();++i)
	{
		if (debug > 5) cerr << "Found child state:" << endl;
		if (debug > 5) showBoard(clearBoard, (*i)->state);

		int newHashMapValue = 0;
		if (addToHashMap(knownStates, (*i), hashMapID, newHashMapValue, false))
		{
			//std::cerr << "New node found, printing..." << std::endl;
			frontier.push(*i);
			
		}
		else if (newHashMapValue != hashMapID)
		{

			if (verbose) cerr << "Goal Found, value " << newHashMapValue << endl;
			frontier.push(*i);
			return true;
		}
	}

	return false;
}

int solveBidirectional(bool verbose, bool notime, int timeout)
{
	Point initialPlayer(0,0);
	Point initialPlayerBack(0,0);

	unordered_map<State, int, StateHash, StateEqual> knownStates;
	Node* start = new Node();
	Node* goal = new BackNode();

	goals = vector<Point>();
	vector<Point> initialBoxes = vector<Point>();

	vector<string> board;
	for (string line; getline(cin, line);)
		board.push_back(line);


	vector<string> backBoard (board);
	vector<string> clear2;
	parseBoard(board, start, goals, clearBoard, false, initialPlayer);
	parseBoard(backBoard, goal, initialBoxes, clear2, true, initialPlayerBack);

	sort(goals.begin(), goals.end());

	sort(initialBoxes.begin(), initialBoxes.end());
	addToHashMap(knownStates, start, 1);
	addToHashMap(knownStates, goal, 2);

	int frontBest = heuristic(start->state);
	int backBest = heuristic(goal->state);

	Node* frontBestNode = start;
	Node* backBestNode = goal;

	priority_queue<Node*, std::vector<Node*>, NodeCompare> frontierFront = priority_queue<Node*, vector<Node*>, NodeCompare>();
	priority_queue<Node*, std::vector<Node*>, NodeCompare> frontierBack = priority_queue<Node*, vector<Node*>, NodeCompare>();

	frontierFront.push(start);
	frontierBack.push(goal);

	clock_t start_clock = clock();

	while(!frontierFront.empty() || !frontierBack.empty())
	{
		if (timeout > 0 && timeout < (clock() - start_clock) /(double) CLOCKS_PER_SEC)
		{
			cout << "Timeout " << timeout << endl;
			return 1;
		}

		if (verbose) cerr << "Expanding back" << endl;
		bool pathFound = expandFrontier(frontierBack, knownStates, 2, verbose, true);
		if (verbose) cerr << "Done." << endl << "Expanding front" << endl;
		pathFound |= expandFrontier(frontierFront, knownStates, 1, verbose, false);
		if (verbose) cerr << "Done." << endl;
		if (pathFound)
		{
			Node* node1;
			Node* node2;
			while (!frontierFront.empty())
			{
				node1 = frontierFront.top();
				// cerr << "Node 1 candidate: " << node1->state.player.toStr() << endl;
				frontierFront.pop();
				if (knownStates[(node1)->state] == 3)
					break;
			}
			while (!frontierBack.empty())
			{
				node2 = frontierBack.top();
				// cerr << "Node 2 candidate: " << node2->state.player.toStr() << endl;

				frontierBack.pop();
				if (knownStates[(node2)->state] == 3)
					break;
			}
			string path1 = getPath(node1);
			// cerr << "Path 1: " << path1 << endl;
			string path2 = getPath(node2);
			path2 = reversePath(path2);
			// cerr << "Path 2: " << path2 << endl;

			
			vector<Point> goals;
			goals.push_back(node2->state.player);
			ProblemSolver findPathToGoals(new MovePlayerToBoxTest(), goals, new NoBoxMoveNode(node1->state,' ',0));

			string pathMiddle = findPathToGoals.findSolution();

			// cerr << "Path from node 1 " << node1->state.player.toStr() << " to node 2 " << node2->state.player.toStr() << ": " << pathMiddle << endl;
		
			cout << path1 << pathMiddle << path2 << endl;
			if (!notime)
				cout << (clock() - start_clock )/(double) CLOCKS_PER_SEC << endl;

			return 0;
		}

	}

	return 1;
}

int solveOneDirection(bool verbose, bool back, bool notime, int timeout)
{
	Point initialPlayer(0,0);

	unordered_map<State, int, StateHash, StateEqual> knownStates;
	Node* start = back ? new BackNode() : new Node();
	

	goals = std::vector<Point>();


	// Read the board
	std::vector<std::string> board;
	for (std::string line; std::getline(std::cin, line);)
		board.push_back(line);

	parseBoard(board, start, goals, clearBoard,back,initialPlayer);
	sort(goals.begin(), goals.end());
	addToHashMap(knownStates, start, 0);
	int best = heuristic(start->state);
	Node* bestNode = start;
	MainGoalTest maintGoalTest = MainGoalTest(back,initialPlayer);

	std::priority_queue<Node*, std::vector<Node*>, NodeCompare> frontier =std::priority_queue<Node*, std::vector<Node*>, NodeCompare>();
	frontier.push(start);
	
	clock_t start_clock = clock();
    //clock_t t = start_clock;
	//int counter = 0;

	while(!frontier.empty())
	{
		if (timeout > 0 && timeout < (clock() - start_clock)/ (double) CLOCKS_PER_SEC)
        {
        	cout << "Timeout " << timeout << endl;
        	return 0;
        }
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
		std::vector<Node*> children = current->getNextSteps();
		if (debug > 1) cerr << "Search over. Children found:  " << children.size() << endl;

		for(std::vector<Node*>::iterator i = children.begin();i!=children.end();++i)
		{
			if(maintGoalTest.isGoal(goals,(*i)->state) )
			{
				vector<Point> goals;
				goals.push_back(initialPlayer);
				ProblemSolver findPathToGoals(new MovePlayerToBoxTest(), goals, new NoBoxMoveNode((*i)->state,' ',0));
				string path = "";
				if (!back || (initialPlayer == (*i)->state.player) || (path=findPathToGoals.findSolution()) != "" )
					{
						std::string answer = getPath(*i) + path;
						//showSolution(clearBoard,start,answer);
						if(back)
							answer = reversePath(answer);
						std::cout << answer << std::endl;
						if(!notime)
							cout << (clock()-start_clock)/(double) CLOCKS_PER_SEC << endl;
						return 0;
					}
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

int main(int argc, const char **argv) {
	bool verbose = false;
	bool back = true;
	bool notime = false;
	bool bidirectional = false;

	debug = 0;
	int timeout = -1;
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
		else if (param == "back" || param == "b")
		{
			back = true;
		}
		else if (param == "-notime")
		{
			notime = true;
		}
		else if (param == "-timeout")
		{
			if (i + 1 < argc)
				timeout = atoi(argv[++i]);
			else
				std::cerr << "Usage: '" << argv[i] << " <int>' " << std::endl;
		}
		else if (param == "-bidirectional" || param == "-bi")
		{
			bidirectional = true;
		}
		else
		{
			std::cerr << "Unknown parameter: '" << argv[i] << "'" << std::endl;
			return -1;
		}
	}
	if (bidirectional)
		back = false;

	if (debug > 0) cerr << "Stating debug level " << debug << endl;

	if (!bidirectional)
		return solveOneDirection(verbose, back, notime, timeout);
	else
	{
		/*int ret = */solveBidirectional(verbose, notime, timeout);
		// if (ret > 0)
		// 	cout << "Solution not found" << endl;

		return 0;
	}

}
