#include <iostream>
#include "sokoban.hpp"

void possibleSteps(std::vector<string> map, Node current, std::vector<Node> &children) {
	// Find possible steps and put into vector. Johan
};

void parseBoard(std::std::vector<stri> map, Node root, std::vector<Point> &goal) {
	// Parse the board and save the current state into currentstate and all possible goal states in the vector
	// Filip
};

int hashState(State state) {
	// Hash state struct to an int. Olli
};

bool isGoal(std::vector<Point> goal, Node node) {
	// Check if the state is goooooal
};

string getPath(Node node) {
	// Returns a string with all steps that led to the state
};

Node findPathTo(Node start, Point goal) {
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
	// std::cout << getPath(goalNode) << std::endl;
	return 0;


	return 0;
}
