#include <vector>
#include <string>
#include <algorithm>

struct Point {
    uint8_t x, y;

    Point():x(0),y(0){}

    Point(uint8_t px, uint8_t py): x(px), y(py){}

    bool operator==(const Point& other) const
	{
    	return x==other.x && y==other.y;
	}
};

struct State {
    Point player;
    std::vector<Point> boxes;

    bool operator==(const State& other){
		//std::pair<std::vector<Point>::iterator ,std::vector<Point>::iterator > theSame = std::mismatch(boxes.begin(),boxes.end(),other.boxes.begin());
		return player==other.player;// && theSame.first!=boxes.end();
    }
};
struct Node {
	State state;
	char direction;
	Node *parent;
};

std::vector<State> possibleSteps(std::vector<std::string> map, State *current);

void parseBoard(std::vector<std::string> &map, Node* root, std::vector<Point> &goal);

int hashState(State state);

bool isGoal(std::vector<Point> goal, State state);

State findPathTo(State start, Point goal);

std::string getPath(State *state);
