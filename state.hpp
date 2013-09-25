#ifndef STATE_H
#define STATE_H

#include <cstdlib>
#include <sstream>
#include <iostream>
#include <string>
#include <tr1/functional>
#include "point.hpp"

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

        for (size_t i = 0; i < boxes.size(); i++) {
            if (boxes[i] != other.boxes[i]) {
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