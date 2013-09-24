#include <vector>
#include <string>

struct Point {
    uint8_t x, y;

    // Define the basic comparisons for hash map purposes.
    bool operator < (const Point& other) const
    {
    	if (y == other.y)
    		return (x < other.x);
        else
        	return (y < other.y);
    }
    bool operator == (const Point& other) const
    {
    	return x == other.x && y == other.y;
    }
    bool operator != (const Point& other) const
    {
    	return !(*this == other);
    }
};

struct State {
    Point player;
    std::vector<Point> boxes;
};

struct Node {
    // TODO: Johan
};

void possibleSteps(std::vector<std::string> map, Node current, std::vector<Node> &children);

void parseBoard(std::vector<std::string> map, Node root, std::vector<Point> &goal);

int hashState(State state);

bool isGoal(std::vector<Point> goal, Node node);

Node findPathTo(Node start, Point goal);

std::string getPath(Node node);
