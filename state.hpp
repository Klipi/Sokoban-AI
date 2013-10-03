#ifndef STATE_H
#define STATE_H

#include <cstdlib>
#include <sstream>
#include <iostream>
#include <string>
#include <tr1/functional>
#include "point.hpp"
#include <algorithm>

struct State{
	Point player;
	std::vector<Point> boxes;

	bool operator==(const State& other) const
	{
		if (player != other.player)
		{
			return false;
		}

		for ( std::vector<Point>::const_iterator i = boxes.begin(); i != boxes.end(); ++i)
		{
			if (std::find(other.boxes.begin(),other.boxes.end(),*i)==other.boxes.end()) {
				return false;
			}
		}

		return true;
	}

	State(){};
	State(Point pl, std::vector<Point> bxs): player(pl), boxes(bxs) {};

};

struct StateHash {
	std::size_t operator()(const State& state) const
	{
		std::ostringstream coordinates;
		coordinates << state.player.x << state.player.y;

		for (size_t i = 0; i < state.boxes.size(); i++)
		{
			coordinates << state.boxes[i].x << state.boxes[i].y;
		}
		std::tr1::hash<std::string> stringHash;
		return stringHash(coordinates.str());
	}
};

struct StateEqual {
	bool operator()(const State& cFirst, const State& cSecond) const
	{
		State first (cFirst);
		State second (cSecond);
		sort(first.boxes.begin(), first.boxes.end());
		sort(second.boxes.begin(), second.boxes.end());
		// std::cerr << "player (" << (int)first.player.x << "," << (int)first.player.y << ") == (" << (int)second.player.x << "," << (int)second.player.y << ") " << std::endl;;
		if (first.player != second.player)
			return false;

		for (size_t i = 0; i != first.boxes.size(); i++)
		{
			// std::cerr << "box (" << (int)first.boxes[i].x << "," << (int)first.boxes[i].y << ") == (" << (int)second.boxes[i].x << "," << (int)second.boxes[i].y << ") ";
			if (first.boxes[i] != second.boxes[i])
			{
				// std::cerr << "Mismatch" << std::endl;
				return false;
			}
			// std::cerr << "Match" << std::endl;
		}

		return true;
	}
};

#endif
