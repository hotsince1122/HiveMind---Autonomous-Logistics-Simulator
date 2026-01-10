#pragma once

#include <vector>
#include <utility>

struct DeliveryPlan{
    int agentIdx = -1;
    int packageIdx = -1;

    std::vector<std::pair<int, int>> path = {};
};