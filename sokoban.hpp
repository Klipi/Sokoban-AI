#include <vector>
#include <string>
#include <algorithm>

struct Point {
    uint8_t x, y;

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

    Point():x(0),y(0){}

    Point(uint8_t px, uint8_t py): x(px), y(py){}
};

struct State {
    Point player;
    std::vector<Point> boxes;

    // Seems broken? Used only for same player position?
    bool operator==(const State& other){
		//std::pair<std::vector<Point>::iterator ,std::vector<Point>::iterator > theSame = std::mismatch(boxes.begin(),boxes.end(),other.boxes.begin());
		if (player != other.player)
        {
            return false;
        }
        if (boxes.size() != other.boxes.size())
        {
            return false;
        }

        for (size_t i = 0; i < boxes.size(); i++) {
            if (boxes[i] != other.boxes[i]) {
                return false;
            }
        }

        return true;
    }
};
struct Node {
	State state;
	char direction;
	Node *parent;
};


std::vector<Node*> possibleSteps(std::vector<std::string> map, State *current);

void parseBoard(std::vector<std::string> &map, Node* root, std::vector<Point> &goal, std::vector<std::string> &clearBoard);

int hashState(State state);

bool isGoal(std::vector<Point> goal, State state);

State findPathTo(State start, Point goal);

std::string getPath(Node* node);

