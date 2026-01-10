#pragma once

#include "agent.h"
#include "../map/map.h"

class Drone : public Agent {

    public:
        Drone(std::pair<int, int> newMapPos);

        bool canTraverse (CellType type) const override;
        char getSymbol() const override;
};