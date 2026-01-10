#include "hivemind_v1.h"
#include "../navigation/navigation.h"

//poate la sfarsit ma uit peste tot proiectul si mai scot din magic numbers XD
struct HiveMindConfig {
    const int rewardMissed = -200;    //(stabilit in pdf -200 daca nu e livrat)
    const int rewardLate = -50;    //(stabilit in pdf -50 daca nu e livrat)
    const int bufferPrecent = 5;
};


std::vector<Route> HiveMindV1::decide (Map& map, std::vector<std::unique_ptr<Agent>>& agents,
    std::vector<std::shared_ptr<Package>>& packages, int currentTick) {

    std::vector<Route> deliveryPlans (packages.size());       //fac lista de rute pentru ficare pachet initializata cu o valoare speciala

    HiveMindConfig HMConfig;


    for(int packageIdx = 0; packageIdx < packages.size(); packageIdx++) {

        auto package = packages[packageIdx];

        int bestPredictedReward = HMConfig.rewardMissed;     //worst case, in care nu trimitem niciun agent, evitand moartea lor 

        if (package->getStatus() != PackageStatus::WAITING)
            continue;


        for(int agentIdx = 0; agentIdx < agents.size(); agentIdx++) {
            auto& agent = agents[agentIdx];

            if(agent->getState() != AgentState::IDLE)
                continue;

            if(agent->getTotalCarriedPackages() >= agent->getTotalPackageCapacity())
                continue;

            if (agent->isAlive() && agent->isIdle()) {
                std::vector<std::pair<int, int>> currentPath = Navigation::getPath(map, map.getHubPosition(), package->getDestiation(), *agent);

                int distanceOneWay = currentPath.size();
                
                int ticksToDestination = distanceOneWay % agent->getSpeed() == 0 ?
                                         distanceOneWay / agent->getSpeed() :
                                         distanceOneWay / agent->getSpeed() + 1;

                int totalTicks =  ticksToDestination * 2;

                int totalBatteryCost = totalTicks * agent->getBatteryPerTick();
                totalBatteryCost += (totalBatteryCost * HMConfig.bufferPrecent) / 100;      //luam marja de 5%

                
                if (totalBatteryCost > agent->getCurrentBattery()) {
                    continue;       //nu luam in considerare trimiterea agentului, evitam pe cat posibil moartea agentiilor
                }

                bool isPlanEmpty = (deliveryPlans[packageIdx].agentIdx == -1);

                if (ticksToDestination > package->getDeadline() && isPlanEmpty) {
                    if (HMConfig.rewardLate > bestPredictedReward) {
                        deliveryPlans[packageIdx].packageIdx = packageIdx;
                        deliveryPlans[packageIdx].agentIdx = agentIdx;
                        deliveryPlans[packageIdx].path = currentPath;
                        bestPredictedReward = HMConfig.rewardLate;
                    }
                    continue;
                }

                int totalCost = totalTicks * agent->getCostPerTick();
                totalCost += (totalCost * HMConfig.bufferPrecent) / 100;     //luam marja de 5%

                int predictedReward = package->getReward() - totalCost;

                if (predictedReward > bestPredictedReward) {
                    deliveryPlans[packageIdx].packageIdx = packageIdx;
                    deliveryPlans[packageIdx].agentIdx = agentIdx;
                    deliveryPlans[packageIdx].path = currentPath;
                    bestPredictedReward = predictedReward;
                }
            }
        }
    }

    return deliveryPlans;
}