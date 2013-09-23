#include <iostream>
#include <queue>
#include <list>
#include "sokoban.hpp"

std::vector<Node*> possibleSteps(std::vector<std::string> map, Node *current) {
	// Find possible steps and put into vector. Johan
	std::vector<Node*> possiblemoves;
	int x = current->state.player.x;
	int y = current->state.player.y;
	int dx[4] = {1, -1, 0, 0};
	int dy[4] = {0, 0, 1, -1};
	char directions[4] = {'R','L','U','D'};
	char antidirections[4] = {'L','R','D','U'};
	Node* childstate = new Node();
	for(int i=0;i<3;i++){
		// check if we move back to the previous state
		if(current->direction == antidirections[i]){
			bool box_pushed = false;
			for(int j = 0; j<current->state.boxes.size();j++){
				if(current->parent->state.boxes[j].x != current->state.boxes[j].x ||
						current->parent->state.boxes[j].y != current->state.boxes[j].y){
					box_pushed = false;
					break;
				}
			}
			if(box_pushed){
				childstate->state.player.x = x+dx[i];
				childstate->state.player.y = y+dy[i];
				childstate->state.boxes = current->state.boxes;
				childstate->direction = directions[i];
				childstate->parent = current;
				possiblemoves.push_back(childstate);
			}
			else if(i==3)
				break;
			else
				i++;
		}

		// check if move is possible
		if(map[x+dx[i]][y+dy[i]] == ' ' || map[x+dx[i]][y+dy[i]] == '.'){
			childstate->state.player.x = x+dx[i];
			childstate->state.player.y = y+dy[i];
			childstate->state.boxes = current->state.boxes;
			childstate->direction = directions[i];
			childstate->parent = current;
			possiblemoves.push_back(childstate);
		}

		// check if push is possible
		else if(map[x+dx[i]][y+dy[i]] != '#'){
			int box_number; // The box that is being pushed
			bool can_push = (map[x+2*dx[i]][y+2*dy[i]] == ' ' || map[x+2*dx[i]][y+2*dy[i]] == '.');
			if(can_push){
				for(int j=0;j<current->state.boxes.size();j++){
					if((x+2*dx[i])==current->state.boxes[j].x &&
							(y+2*dy[i])==current->state.boxes[j].y){
						can_push = false;
						break;
					}
					else if((x+dx[i])==current->state.boxes[j].x &&
							(y+dy[i])==current->state.boxes[j].y){
						box_number = j;
					}
				}
			}
			if(can_push){
				childstate->state.player.x = x+dx[i];
				childstate->state.player.y = y+dy[i];
				childstate->state.boxes = current->state.boxes;
				childstate->state.boxes[box_number].x = x+2*dx[i];
				childstate->state.boxes[box_number].y = y+2*dy[i];
				childstate->direction = directions[i];
				childstate->parent = current;
				possiblemoves.push_back(childstate);
			}
		}

	}
	return possiblemoves;
};

void parseBoard(std::vector<std::string> &map, Node* root, std::vector<Point> &goal) {
	// Parse the board and save the current state into currentstate and all possible goal states in the vector
	// Filip
	for (uint8_t i = 0; i<map.size();++i)
	{
		//std::cout<<board[i]<<std::endl;
		uint8_t x;
		int p=0;
		while((x=map[i].find('$',p))<std::string::npos)
		{
			p=x+1;
			root->state.boxes.push_back(Point(x,i));
		}

		while((x=map[i].find('.',p))<std::string::npos)
		{
			p=x+1;
			goal.push_back(Point(x,i));
		}

		while((x=map[i].find('*',p))<std::string::npos)
		{
			p=x+1;
			goal.push_back(Point(x,i));
			root->state.boxes.push_back(Point(x,i));
		}

		if((x=map[i].find('@'))<std::string::npos || (x=map[i].find('+'))<std::string::npos)
			root->state.player = Point(x,i);
	}
};

int hashState(State state) {
	// Hash state struct to an int. Olli
};

bool isGoal(std::vector<Point> goal, State state) {
	// Check if the state is goooooal
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

State findPathTo(State start, Point goal) {
	/* Find the path for the player to the goal point. Creates the child nodes
	   and returning the last one if reached the goal TODO: Olli? */
}

int main(int argc, char **argv) {
	// Implement hash table, Olli

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

	// Output answer
	// std::cout << getPath(goalState) << std::endl;
	return 0;

}
