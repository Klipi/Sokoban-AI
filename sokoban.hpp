#include <vector>
#include <string>

struct Point {
    uint8_t x, y;
};

struct State {
    Point player;
    std::vector<Point> boxes;
    char direction;
    State *parent;
};

std::vector<State> possibleSteps(std::vector<string> map, State *current);

void parseBoard(std::vector<string> map, State root, std::vector<Point> &goal);

int hashState(State state);

bool isGoal(std::vector<Point> goal, State state);

State findPathTo(State start, Point goal);

std::string getPath(State *state);
