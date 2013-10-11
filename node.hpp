#ifndef NODE_H
#define NODE_H

#include <vector>
#include "state.hpp"
#include "point.hpp"

class Node {
	public:
		State state;
		std::string direction;
		Node *parent;

		bool hasBoxIn(const Point&);
		bool hasWallIn(const Point&);
		bool hasGoalIn(const Point&);
		virtual bool isFreePoint(const Point&);
		Node* getChild(char, bool);
		//bool identifyDeadBox(Node*, Point);
		bool isBoxStuck(Point, bool);
		bool isBoxStuck(Point);
		//bool identifyDeadBox(Point);
		bool identifyDeadGroup(std::vector<Point>, Point);
		std::vector<Point> getAdjacentBoxGroup(Point);

		Node():direction(1,' '),parent(NULL){};
		Node(State s, char d, Node* p):state(s), direction(1,d), parent(p){};
		Node(State s, std::string d, Node* p):state(s), direction(d), parent(p){};
		virtual ~Node(){};

		virtual std::vector<Node*> getNextSteps(const std::vector<std::string> &map);
		std::vector<Point> getMovableSides(Point box, std::vector<std::string> map);
		std::vector<Node*> findPaths(std::vector<Point> goals, std::vector<std::string> map);
		bool isSearchTarget(std::vector<Point> &goals);

};

int heuristic(const State&);

struct NodeCompare
{
	bool operator()(const Node* a, const Node* b) const
	{
		return heuristic(a->state) > heuristic(b->state);
	}
};

class BackNode: public Node
{
public:
	BackNode():Node(){};
	BackNode(State s, char d, Node* p):Node(s, d, p){};
	virtual ~BackNode(){};
	bool isFreePoint(const Point&);
	std::vector<Node*> getNextSteps(const std::vector<std::string> &map);
};

class NoBoxMoveNode: public Node
{
public:
	NoBoxMoveNode():Node(){};
	NoBoxMoveNode(State s, char d, Node* p):Node(s, d, p){};
	virtual ~NoBoxMoveNode(){};
	bool isFreePoint(const Point&);
	std::vector<Node*> getNextSteps(const std::vector<std::string> &map);

};
#endif
