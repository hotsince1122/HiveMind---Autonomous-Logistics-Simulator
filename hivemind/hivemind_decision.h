#pragma once

#include <vector>
#include <utility>

struct Route{
    int agentIdx = -1;
    int packageIdx = -1;

    std::vector<std::pair<int, int>> path = {};
};

struct PathEval {
    int ticks;
    int batteryCost;
    int moneyCost;
};