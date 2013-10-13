/*
 * goalTest.hpp
 *
 *  Created on: 8 paü 2013
 *      Author: filip
 */

#ifndef GOALTEST_HPP_
#define GOALTEST_HPP_

class GoalTest
{
public:
	virtual ~GoalTest(){};
	virtual bool isGoal(std::vector<Point> &goals, State &state) = 0;
};

class MovePlayerToBoxTest: public GoalTest
{
	bool isGoal(std::vector<Point> &goals, State &state)
	{
		for(unsigned int i =0; i< goals.size();++i)
	        if (state.player == goals[i])
	        	return true;
		return false;
	}

};

class MainGoalTest: public GoalTest
{
	bool back;
	const Point &initialPlayerPosition;
public:
	MainGoalTest(bool direction, const Point &playerPos):back(direction),initialPlayerPosition(playerPos){};
	~MainGoalTest(){};

	bool isGoal(std::vector<Point> &goal, State &state)
	{
		// Assume goal and boxes in state is sorted
		for (size_t i = 0; i < state.boxes.size(); i++)
		{
			if (goal[i] != state.boxes[i])
			{
				return false;
			}
		}
		if(back)
			return state.player==initialPlayerPosition;
		return true;
	};

};



#endif /* GOALTEST_HPP_ */
