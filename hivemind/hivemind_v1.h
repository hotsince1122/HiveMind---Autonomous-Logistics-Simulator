#pragma once

#include "../map/map.h"
#include "../agents/agent.h"
#include "../orders/package.h"
#include "hivemind_decision.h"

#include <utility>
#include <vector>
#include <memory>

class HiveMindV1 {

    public:

        static std::vector<DeliveryPlan> decide (Map& map, std::vector<std::unique_ptr<Agent>>& agents,
            std::vector<std::shared_ptr<Package>>& packages, int currentTick);
};