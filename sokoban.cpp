#include <iostream>
#include <unordered_map>
#include <string>
#include <sstream>
#include <tr1/functional>
#include <algorithm>
#include "sokoban.hpp"

using namespace std;

void possibleSteps(std::vector<string> map, Node current, std::vector<Node> &children) {
	// Find possible steps and put into vector. Johan
};

void parseBoard(std::vector<string> map, Node root, std::vector<Point> &goal) {
	// Parse the board and save the current state into currentstate and all possible goal states in the vector
	// Filip
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

// This comparison will sort the State.boxes vector as a side-effect.
struct KeyEqual {
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

bool isGoal(std::vector<Point> goal, Node node) {
	// Check if the state is goooooal
	return false;
};

string getPath(Node node) {
	// Returns a string with all steps that led to the state
	return "";
};

Node findPathTo(Node start, Point goal) {
	/* Find the path for the player to the goal point. Creates the child nodes
	   and returning the last one if reached the goal TODO: Olli? */
	return Node();
};

int main(int argc, const char **argv) {
	//unordered_map<State, int, StateHash, StateEqual> knownStates;
	//std::unordered_map<State, int, StateHash, StateEqual> m6;

	// Read the board
	std::vector<std::string> board;
	for (std::string line; std::getline(std::cin, line);)
		board.push_back(line);

	// TODO: Find path to goal
	// Implement algoritm. Filip

	// Output answer
	// std::cout << getPath(goalNode) << std::endl;
	return 0;
};
