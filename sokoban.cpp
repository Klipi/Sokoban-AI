#include <iostream>
#include <string>
#include <vector>

int main(int argc, char **argv)
{
	// Read the board
	std::vector<std::string> board;
	for (std::string line; std::getline(std::cin, line);)
		board.push_back(line);

	// TODO: Find path to goal

	// Output answer
	std::cout << "U R R U" << std::endl;
	return 0;
}
