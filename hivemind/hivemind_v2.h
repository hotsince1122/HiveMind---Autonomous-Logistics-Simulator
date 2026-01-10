#pragma once

#include "../map/map.h"
#include "../agents/agent.h"
#include "../orders/package.h"
#include "hivemind_decision.h"

#include <utility>
#include <vector>
#include <memory>

class HiveMindV2 {

    struct HiveMindConfig {
        const int rewardMissed = -200;    //(stabilit in pdf -200 daca nu e livrat)
        const int rewardLate = -50;    //(stabilit in pdf -50 daca nu e livrat)
        const int bufferPrecent = 5;
    };

    static inline constexpr HiveMindConfig config = { -200, -50, 2 };

    static int computeTicksForPath (const std::vector<std::pair<int, int>>& path, int speed);
    static int applyBuffer (int value, int bufferPercent);
    static PathEval evaluatePath (const std::vector<std::pair<int, int>>& path, Agent& agent, int bufferPercent);

    static std::vector<std::pair<int, int>> nearestStation (Map& map, Agent& agent, std::pair<int, int> currentPos);
    static std::vector<std::pair<int, int>> nearestStationToHub (Map& map, Agent& agent, std::pair<int, int> currentPos);

    public:
    
        static std::vector<Route> decideDelivery (Map& map, std::vector<std::unique_ptr<Agent>>& agents,
            std::vector<std::shared_ptr<Package>>& packages, int currentTick);

        static std::vector<Route> decideReturn (Map& map, std::vector<std::unique_ptr<Agent>>& agents);

};