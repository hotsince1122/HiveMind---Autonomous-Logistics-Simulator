#pragma once
#include <vector>
#include <queue>
#include <algorithm>
#include "../map/map.h"
#include "../agents/agent.h"

class Navigation {
public:

    static std::vector<std::pair<int, int>> getPath(
        const Map& map, 
        std::pair<int, int> start, 
        std::pair<int, int> end, 
        const Agent& agent
    );
};