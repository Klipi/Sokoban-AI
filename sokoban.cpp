#include <iostream>
#include <unordered_map>
#include <string>
#include <sstream>
#include <tr1/functional>
#include <algorithm>
#include <queue>
#include <list>
#include "sokoban.hpp"

using namespace std;

std::vector<Node*> possibleSteps(std::vector<std::string> map, Node *current) {
	// Find possible steps and put into vector. Johan
	std::vector<Node*> possiblemoves;
	int x = current->state.player.x;
	int y = current->state.player.y;
	int dx[4] = {1, -1, 0, 0};
	int dy[4] = {0, 0, 1, -1};
	char directions[4] = {'R','L','D','U'};
	Node* childstate = new Node();
	Point new_pos;
	
	for(size_t i=0;i<3;i++)
	{
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
					y+2*dy[i] < map.size() && x+2*dx[i] < map[y+2*dy[i]].size() &&
					map[y+2*dy[i]][x+2*dx[i]] != '#' && 
			      	  	(std::find(current->state.boxes.begin(), current->state.boxes.end(), new_box_pos) == current->state.boxes.end()))
			{	
				childstate->state.player = new_pos;
				childstate->state.boxes = current->state.boxes;
				childstate->state.boxes[std::distance(current->state.boxes.begin(),pushed_box)] = new_box_pos;
				childstate->direction = directions[i];
				childstate->parent = current;
				possiblemoves.push_back(childstate);
			}
		}
			
		// check if move is possible
		else if(		y+dy[i] > 0 && x+dx[i] > 0 &&
					y+dy[i] < map.size() && x+dx[i] < map[y+dy[i]].size() &&
					map[y+dy[i]][x+dx[i]] != '#'){
			childstate->state.player = new_pos;
			childstate->state.boxes = current->state.boxes;
			childstate->direction = directions[i];
			childstate->parent = current;
			possiblemoves.push_back(childstate);
		}

	}
	return possiblemoves;
};


void parseBoard(std::vector<std::string> &map, Node* root, std::vector<Point> &goal) {
	// Parse the board and save the current state into currentstate and all possible goal states in the vector
	// Filip
	for (uint8_t i = 1; i<map.size()-1;++i)
	{
		int x;
		int p=0;

		while((x=map[i].find('$',p)) < std::string::npos)
		{
			p=x+1;
			root->state.boxes.push_back(Point(x,i));
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
		}
	
		if((x=map[i].find('@')) < std::string::npos || (x=map[i].find('+')) < std::string::npos)
			root->state.player = Point(x,i);
	}
//	std::cout<<"player: "<<root->state.player.x<<":"<<root->state.player.y<<std::endl;
//	std::cout<<"boxes:"<<root->state.boxes.size()<<std::endl;
//	for(int i = 0; i< root->state.boxes.size();++i)
//	{
//		std::cout<<root->state.boxes[i].x<<":"<<root->state.boxes[i].y<<std::endl;
//	}
//	std::cout<<"goals:"<<goal.size()<<std::endl;
//	for(int i = 0; i< goal.size();++i)
//	{
//		std::cout<<goal[i].x<<":"<<goal[i].y<<std::endl;
//	}
};

// Hash state struct to a size_t. Olli
struct StateHash {
 std::size_t operator()(const State& state) const
 {
 	ostringstream coordinates;
	coordinates << state.player.x << state.player.y;

	for (size_t i = 0; i < state.boxes.size(); i++)
	{
		coordinates << state.boxes[i].x << state.boxes[i].y;
	}
	std::tr1::hash<string> stringHash;
    return stringHash(coordinates.str());
 }
};

struct StateEqual {
	bool operator()(const State& cFirst, const State& cSecond) const
	{
		State first (cFirst);
		State second (cSecond);
		std::sort(first.boxes.begin(), first.boxes.end());
		std::sort(second.boxes.begin(), second.boxes.end());
		if (first.boxes.size() != second.boxes.size())
			return false;

		if (first.player != second.player)
			return false;

		for (size_t i = 0; i != first.boxes.size(); i++)
		{
			if (first.boxes[i] != second.boxes[i])
				return false;
		}

	    return true;
	}
};

bool isGoal(std::vector<Point> goal, State state) {
	// Asume goal is sorted
	std::sort(state.boxes.begin(), state.boxes.end());

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


int main(int argc, const char **argv) {
	unordered_map<State, int, StateHash, StateEqual> knownStates;

	// Read the board
	std::vector<std::string> board;
	for (std::string line; std::getline(std::cin, line);)
		board.push_back(line);
	Node* start = new Node();
	std::vector<Point> goal = std::vector<Point>();
	parseBoard(board, start, goal);
	// TODO: Find path to goal
	// Implement algoritm. Filip

	std::priority_queue<Node*> frontier = std::priority_queue<Node*>();
	frontier.push(start);
	std::list<State> explored = std::list<State>();

	while(!frontier.empty())
	{
		Node* current = frontier.top();
		explored.push_back(current->state);
		frontier.pop();
		std::vector<Node*> children = possibleSteps(board,current);
		for(std::vector<Node*>::iterator i = children.begin();i!=children.end();++i)
		{
			if(isGoal(goal,(*i)->state))
			{
				std::string answer = getPath(*i);
				std::cout << answer << std::endl;
				return 0;
			}
			if (std::find(explored.begin(),explored.end(),(*i)->state)==explored.end())
			{
				frontier.push(*i);
			}
		}
	}

	return 0;
}
