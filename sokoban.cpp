#include <iostream>
#include "sokoban.hpp"

std::vector<State> possibleSteps(std::vector<string> map, State current) {
	// Find possible steps and put into vector. Johan
	std::vector<State> possiblemoves;
        int x = current.player.x;
        int y = current.player.y;
        int dx[4] = {1, -1, 0, 0};
        int dy[4] = {0, 0, 1, -1};
        char directions[4] = {'R','L','U','D'};
        char antidirections[4] = {'L','R','D','U'};
        State childstate;
        for(int i=0;i<3;i++){
        	// check if we move to the previous state
        	if(current.direction == antidirection[i] && current.parent.boxes == current.boxes)
        		void;
        	
       		// check if move is possible
                else if(map[x+dx[i]][y+dy[i]] == ' ' || map[x+dx[i]][y+dy[i]] == '.'){
                	childstate.player.x = x+dx[i];
                        childstate.player.y = y+dy[i];
                        childstate.boxes = current.boxes;
                        childstate.direction = directions[i];
                        childstate.parent = current;
                        possiblemoves.push_back(childstate);
                }
                           
                // check if push is possible
                else if(map[x+dx[i]][y+dy[i]] != '#'){
                	int box_number; // The box that is being pushed
                        bool can_push = (map[x+2*dx[i]][y+2*dy[i]] == ' ' || map[x+2*dx[i]][y+2*dy[i]] == '.');
                        if(can_push){
                         	for(int j=0;j<current.boxes.size();j++){
                                       if((x+2*dx[i])==current.boxes[j].x && 
                                          (y+2*dy[i])==current.boxes[j].y){
                                              can_push = false;
                                              break;
                                       }
                                       else if((x+dx[i])==current.boxes[j].x && 
                                               (y+dy[i])==current.boxes[j].y){
                                              box_number = j;
                                       }
                                   }
                               }
                         if(can_push){
                          	childstate.player.x = x+dx[i];
                                childstate.player.y = y+dy[i];
                               	childstate.boxes = current.boxes;
                                childstate.boxes[box_number].x = x+2*dx[i];
                                childstate.boxes[box_number].y = y+2*dy[i];
                                childstate.direction = directions[i];
                        	childstate.parent = current;
                                possiblemoves.push_back(childstate);
                         }
                }
                           
         }
};

void parseBoard(std::vector<string> map, State root, std::vector<Point> &goal) {
	// Parse the board and save the current state into currentstate and all possible goal states in the vector
	// Filip
};

int hashState(State state) {
	// Hash state struct to an int. Olli
};

bool isGoal(std::vector<Point> goal, State state) {
	// Check if the state is goooooal
};

std::string getPath(State *state) {
	// Returns a string with all steps that led to the state
	std::string path;
       	while(state->parent != NULL){
         	path.insert(path.begin(),state->direction);
                state = state->parent;
       	}
       	return path;
};

Node findPathTo(State start, Point goal) {
	/* Find the path for the player to the goal point. Creates the child nodes
	   and returning the last one if reached the goal TODO: Olli? */
}

int main(argc, **argv) {
	// Implement hash table, Olli

	// Read the board
	std::vector<std::string> board;
	for (std::string line; std::getline(std::cin, line);)
		board.push_back(line);

	// TODO: Find path to goal
	// Implement algoritm. Filip

	// Output answer
	// std::cout << getPath(goalState) << std::endl;
	return 0;


	return 0;
}
