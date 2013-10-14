#ifndef NODE_H
#define NODE_H

#include <vector>
#include "state.hpp"
#include "point.hpp"

int heuristic(const State&);

class Node {
	public:
		State state;
		char direction;
		int hCost;
		Node *parent;

		bool hasBoxIn(const Point&);
		bool hasWallIn(const Point&);
		bool hasGoalIn(const Point&);
		bool isWalkable(const Point&);
		virtual bool isFreePoint(const Point&);
		Node* getChild(char, bool);
		//bool identifyDeadBox(Node*, Point);
		bool isBoxStuck(Point, bool);
		bool isBoxStuck(Point);
		//bool identifyDeadBox(Point);
		bool identifyDeadGroup(std::vector<Point>, Point);
		std::vector<Point> getAdjacentBoxGroup(Point);

		Node():direction(' '),hCost(0),parent(NULL){};
		Node(State s, char d, Node* p):state(s), direction(d), parent(p){
			hCost = heuristic(state);
		};
		virtual ~Node(){};

		virtual std::vector<Node*> getNextSteps();
		std::vector<Point> getMovableSides(Point box, std::vector<std::string> map);
		std::vector<Node*> findPaths(std::vector<Point> goals, std::vector<std::string> map);
		bool isSearchTarget(std::vector<Point> &goals);

};

struct NodeCompare
{
	bool operator()(const Node* a, const Node* b) const
	{
		return a->hCost>b->hCost;
	}
};

class BackNode: public Node
{
public:
	BackNode():Node(){};
	BackNode(State s, char d, Node* p):Node(s, d, p){};
	virtual ~BackNode(){};
	bool isFreePoint(const Point&);
	std::vector<Node*> getNextSteps();
};

class NoBoxMoveNode: public Node
{
public:
	NoBoxMoveNode():Node(){};
	NoBoxMoveNode(State s, char d, Node* p):Node(s, d, p){};
	virtual ~NoBoxMoveNode(){};
	bool isFreePoint(const Point&);
	std::vector<Node*> getNextSteps();

};
#endif
