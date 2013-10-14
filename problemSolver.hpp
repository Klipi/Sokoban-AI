/*
 * SolveProblem.hpp
 *
 *  Created on: 8 paü 2013
 *      Author: filip
 */

#ifndef SOLVEPROBLEM_HPP_
#define SOLVEPROBLEM_HPP_
#include <unordered_map>
#include "goalTest.hpp"
#include "sokoban.hpp"

class ProblemSolver
{
	GoalTest* goalTest;
	std::vector<Point> &goals;
	std::unordered_map<State, int, StateHash, StateEqual> knownStates;
	std::priority_queue<Node*, std::vector<Node*>, NodeCompare> frontier;

	bool addToHashMap(Node* node, int value){

		if (knownStates.find(node->state) == knownStates.end())
		{
			knownStates.insert({node->state, value});
			return true;
		}
		else
		{
			knownStates[node->state] = value;
			return false;
		}

	}

public:
	ProblemSolver(GoalTest* gT, std::vector<Point>& g, Node* start):goalTest(gT),goals(g)
	{
		addToHashMap(start, 0);
		frontier.push(start);
	};
	virtual ~ProblemSolver(){};
	virtual std::string findSolution()
	{
		while(!frontier.empty())
		{
			Node* current = frontier.top();
			frontier.pop();
			std::vector<Node*> children = current->getNextSteps();

			for(std::vector<Node*>::iterator i = children.begin();i!=children.end();++i)
			{
				if(goalTest->isGoal(goals,(*i)->state))
				{
					return getPath(*i);
				}

				if (addToHashMap((*i), 0))
				{
					frontier.push(*i);
				}
			}
		}
		return "";
	}
};



#endif /* SOLVEPROBLEM_HPP_ */
