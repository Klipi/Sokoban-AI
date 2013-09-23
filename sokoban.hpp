#include <vector>
#include <string>

struct Point {
    uint8_t x, y;
};

struct State {
    Point player;
    std::vector<Point> boards;
};

struct Node {
    // TODO: Johan
};

void possibleSteps(std::vector<string> map, Node current, std::vector<Node> &children);

void parseBoard(std::std::vector<stri> map, Node root, std::vector<Point> &goal);

int hashState(State state);

bool isGoal(std::vector<Point> goal, Node node);

Node findPathTo(Node start, Point goal);

string getPath(Node node);
