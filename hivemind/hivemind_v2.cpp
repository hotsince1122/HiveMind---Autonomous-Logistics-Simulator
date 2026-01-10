#include "hivemind_v2.h"
#include "../navigation/navigation.h"

//poate la sfarsit ma uit peste tot proiectul si mai scot din magic numbers XD
struct HiveMindConfig {
    const int rewardMissed = -200;    //(stabilit in pdf -200 daca nu e livrat)
    const int rewardLate = -50;    //(stabilit in pdf -50 daca nu e livrat)
    const int bufferPrecent = 2;
};


std::vector<DeliveryPlan> HiveMindV2::decide (Map& map, std::vector<std::unique_ptr<Agent>>& agents,
    std::vector<std::shared_ptr<Package>>& packages, int currentTick) {

    std::vector<bool> agentIsAssigned(agents.size(), false);

    std::vector<DeliveryPlan> deliveryPlans (packages.size());       //fac lista de rute pentru ficare pachet initializata cu o valoare speciala

    std::vector<std::pair<int, int>> safeSpots = map.getStations();     //ca sa poata sa faca drumuri mai risky, fie pana la hub
    safeSpots.push_back(map.getHubPosition());                          //fie cea mai apropiata statie de incarcare

    HiveMindConfig HMConfig;

    for(int packageIdx = 0; packageIdx < packages.size(); packageIdx++) {

        auto package = packages[packageIdx];

        int bestPredictedReward = HMConfig.rewardMissed;     //worst case, in care nu trimitem niciun agent, evitand moartea lor 
        int bestAgentIdx = -1;

        if (package->getStatus() != PackageStatus::WAITING && package->getStatus() != PackageStatus::EXPIRED)   //duce si pachetele expirate (evitam -200)
            continue;


        for(int agentIdx = 0; agentIdx < agents.size(); agentIdx++) {
            auto& agent = agents[agentIdx];

            if(agent->getState() != AgentState::IDLE || agentIsAssigned[agentIdx])
                continue;

            if(agent->getTotalCarriedPackages() >= agent->getTotalPackageCapacity())
                continue;

            if (agent->isAlive()) {
                std::vector<std::pair<int, int>> currentPath = Navigation::getPath(map, map.getHubPosition(), package->getDestiation(), *agent);

                int distanceOneWay = currentPath.size();
                
                int ticksToDestination = distanceOneWay / agent->getSpeed();
                if (distanceOneWay % agent->getSpeed() != 0)
                    ticksToDestination++;

                int minTicksBack = 99999999;

                for (const auto& spot : safeSpots) {        //cautam cel mai apropiat safespot
                    auto pathBack = Navigation::getPath(map, package->getDestiation(), spot, *agent);
                    if (pathBack.empty()) continue;

                    int ticks = pathBack.size() / agent->getSpeed();
                    if (pathBack.size() % agent->getSpeed() != 0) ticks++;

                    if (ticks < minTicksBack) {
                        minTicksBack = ticks;
                    }
                }

                if (minTicksBack == 99999999) continue; // nu are unde sa se retraga

                int totalTicks = ticksToDestination + minTicksBack;

                int totalBatteryCost = totalTicks * agent->getBatteryPerTick();
                totalBatteryCost += (totalBatteryCost * HMConfig.bufferPrecent) / 100;      //luam marja de 5%

                
                if (totalBatteryCost > agent->getCurrentBattery()) {
                    continue;       //nu luam in considerare trimiterea agentului, evitam pe cat posibil moartea agentiilor
                }

                bool isPlanEmpty = (bestAgentIdx == -1);

                if (ticksToDestination > package->getDeadline() && isPlanEmpty) {
                    if (HMConfig.rewardLate > bestPredictedReward) {
                        deliveryPlans[packageIdx].packageIdx = packageIdx;
                        deliveryPlans[packageIdx].agentIdx = agentIdx;
                        deliveryPlans[packageIdx].path = currentPath;
                        bestPredictedReward = HMConfig.rewardLate;

                        bestAgentIdx = agentIdx;
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
                    
                    bestAgentIdx = agentIdx;
                }
            }
        }

        if (bestAgentIdx != -1 && bestPredictedReward > HMConfig.rewardMissed) {
            agentIsAssigned[bestAgentIdx] = true;
        }
    }

    return deliveryPlans;
}