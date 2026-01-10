#pragma once

#include "agent.h"
#include "../map/map.h"

class Robot : public Agent {

    public:
        Robot(std::pair<int, int> newMapPos);

        bool canTraverse (CellType type) const override;
        char getSymbol() const override;
};