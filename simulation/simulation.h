#pragma once

#include "../map/map.h"
#include "../agents/agent.h"
#include "../orders/package.h"
#include "../map/config.h"
// #include "../hivemind/hivemind_v1.h"
#include "../hivemind/hivemind_v2.h"

#include <vector>
#include <memory>
#include <random>

class Simulation {
    Map& map;
    std::vector<std::unique_ptr<Agent>> agents;
    std::vector<std::shared_ptr<Package>> packages;
    HiveMindV2 hiveMind;

    int currentTick;
    int maxTicks;
    
    int spawnedPackages = 0;
    int totalPackages;
    int spawnFreq;

    int totalProfit = 0;
    int deliveredCount = 0;
    int lateCount = 0;
    int unDeliveredCount = 0;
    int deadAgents = 0;

    public:
        Simulation (Map& newMap, const Config& configs);

        void run(const Config& configs);

    private:
        void spawnAgentFleet(const Config& configs, std::pair<int, int> hub);
        void spawnPackagesIfNeeded(std::mt19937& gen, const std::vector<std::pair<int, int>>& destinations);
        void updatePackages();
        void updateAgents(std::vector<Route> deliveryPlans, std::vector<Route> returnPlans);
};