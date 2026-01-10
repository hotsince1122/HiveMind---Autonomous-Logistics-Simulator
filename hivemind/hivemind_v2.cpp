#include "hivemind_v2.h"
#include "../navigation/navigation.h"

int HiveMindV2::computeTicksForPath (const std::vector<std::pair<int, int>>& path, int speed) {
    if (path.empty())
        return 0;

    int ticks = path.size() / speed;
    if (path.size() % speed != 0)
        ticks++;

    return ticks;
}

int HiveMindV2::applyBuffer (int value, int bufferPercent) {
    return value + (value * bufferPercent) / 100;
}

PathEval HiveMindV2::evaluatePath (const std::vector<std::pair<int, int>>& path, Agent& agent, int bufferPercent) {
    PathEval result;

    result.ticks = computeTicksForPath(path, agent.getSpeed());

    result.batteryCost = applyBuffer(
        result.ticks * agent.getBatteryPerTick(),
        bufferPercent
    );

    result.moneyCost = applyBuffer(
        result.ticks * agent.getCostPerTick(),
        bufferPercent
    );

    return result;
}

std::vector<std::pair<int, int>> HiveMindV2::nearestStation (Map& map, Agent& agent, std::pair<int, int> currentPos) {
    int minTicksBack = INT_MAX;

    std::vector<std::pair<int, int>> nearestStationPath;

    for (const auto& station : map.getStations()) {
        auto path = Navigation::getPath(map, currentPos, station, agent);

        if (path.empty())
            continue;

        PathEval eval = evaluatePath(path, agent, config.bufferPrecent);
        if (agent.getCurrentBattery() < eval.batteryCost)
            continue;


        if (eval.ticks < minTicksBack) {     //cautam cel mai apropiat safeSpot
            minTicksBack = eval.ticks;
            nearestStationPath = path;
        }       
    }

    return nearestStationPath;      // gol daca nu exista
}

std::vector<std::pair<int, int>> HiveMindV2::nearestStationToHub (Map& map, Agent& agent, std::pair<int, int> currentPos) {

    int bestTicks = INT_MAX;
    std::vector<std::pair<int, int>>  bestPath;

    auto hubPos = map.getHubPosition();

    int agentToHubDist = abs(currentPos.first - hubPos.first) + abs(currentPos.second - hubPos.second);


    //cautam statia care e cea mai "in drum" spre hub
    for (const auto& station : map.getStations()) {

        int stationToHubDist = abs(station.first - hubPos.first) + abs(station.second - hubPos.second);

        if (stationToHubDist >= agentToHubDist)     //statia nu e spre hub
            continue;

        auto pathToStation = Navigation::getPath(map, currentPos, station, agent);

        if (pathToStation.empty())
            continue;

        PathEval pathEval = evaluatePath(pathToStation, agent, config.bufferPrecent);

        int ticks = pathEval.ticks;
        int batteryCost = pathEval.batteryCost;

        if (ticks < bestTicks && agent.getCurrentBattery() >= batteryCost) {
            bestTicks = ticks;
            bestPath = pathToStation;
        }
    }

    if(!bestPath.empty())
        return bestPath;
    else {
        //daca agentul nu are bateria necesara sa ajunga la o statie din drum, merge la cea mai apropiata statie
        bestPath = nearestStation(map, agent, currentPos);
    }

    return bestPath;
}

std::vector<Route> HiveMindV2::decideDelivery (Map& map, std::vector<std::unique_ptr<Agent>>& agents,
    std::vector<std::shared_ptr<Package>>& packages, int currentTick) {

    std::vector<int> virtualLoad(agents.size(), 0);     //putem asigna mai multe pachete unui agent, cu conditia ca nu ii depasesc capacitatea

    std::vector<Route> deliveryPlans(packages.size());

    std::vector<std::pair<int, int>> safeSpots = map.getStations();
    safeSpots.push_back(map.getHubPosition());

    for (int packageIdx = 0; packageIdx < packages.size(); packageIdx++) {
        auto package = packages[packageIdx];

        int bestPredictedReward = config.rewardMissed;
        int bestAgentIdx = -1;

        if (package->getStatus() != PackageStatus::WAITING && package->getStatus() != PackageStatus::EXPIRED)
            continue;

        for (int agentIdx = 0; agentIdx < agents.size(); agentIdx++) {
            auto& agent = agents[agentIdx];

            if (agent->getState() != AgentState::IDLE)
                continue;

            if (agent->getTotalCarriedPackages() + virtualLoad[agentIdx] >= agent->getTotalPackageCapacity())
                continue;

            if (!agent->isAlive())
                continue;

            std::vector<std::pair<int, int>> currentPath = Navigation::getPath(map, agent->getMapPos(), package->getDestiation(),  *agent);

            if (currentPath.empty())        //teoretic nu are cum sa se intample asta, dar sa fim siguri
                continue;

            PathEval forwardEval = evaluatePath(currentPath, *agent, config.bufferPrecent);

            auto pathBack = nearestStationToHub (map, *agent, package->getDestiation());

            if (pathBack.empty()) {       //agentul nu poate ajunge inapoi
                pathBack = nearestStation (map, *agent, package->getDestiation());
                if (pathBack.empty())
                    continue;
            }

            PathEval returnEval = evaluatePath(pathBack, *agent, config.bufferPrecent);

            int totalTicks = forwardEval.ticks + returnEval.ticks;

            int totalBatteryCost = totalTicks * agent->getBatteryPerTick();

            if (totalBatteryCost > agent->getCurrentBattery())      // agentul nu are suficienta baterie pt drum
                continue;

            int totalCost = applyBuffer( totalTicks * agent->getCostPerTick(), config.bufferPrecent );

            int predictedReward;
            if (forwardEval.ticks > package->getDeadline())       //trimitem primul agent care poate ajunge acolo doar sa nu luam -200
                predictedReward = config.rewardLate;
            else predictedReward = package->getReward() - totalCost;

            if (predictedReward > bestPredictedReward) {
                deliveryPlans[packageIdx].packageIdx = packageIdx;
                deliveryPlans[packageIdx].agentIdx = agentIdx;
                deliveryPlans[packageIdx].path = currentPath;

                bestPredictedReward = predictedReward;
                bestAgentIdx = agentIdx;
            }
        }

        if (bestAgentIdx != -1 && bestPredictedReward > config.rewardMissed) {
            virtualLoad[bestAgentIdx]++;
        }
    }

    return deliveryPlans;
}


std::vector<Route> HiveMindV2::decideReturn (Map& map, std::vector<std::unique_ptr<Agent>>& agents) {

    std::vector<Route> returnPlan;

    for (int agentIdx = 0; agentIdx < agents.size(); agentIdx++) {
        auto& agent = agents[agentIdx];

        if(!agent->isAlive())
            continue;

        CellType currentCell = map.getCell(agent->getMapPos());

        if(agent->isIdle() && !agent->hasPackages()) {   //daca a terminat de livrat tot si nu e la incarcat

            if (currentCell == CellType::DESTINATION || currentCell == CellType::STATION) {      //este la o desinatie

                std::vector<std::pair<int, int>> pathToHub;
                
                pathToHub = Navigation::getPath(map, agent->getMapPos(), map.getHubPosition(), *agent);

                PathEval pathToHubEval = evaluatePath(pathToHub, *agent, config.bufferPrecent);

                if(agent->getCurrentBattery() >= pathToHubEval.batteryCost) {      //are suficienta baterie sa se intoarca la hub
                    Route newReturnPlan;
                    newReturnPlan.agentIdx = agentIdx;
                    newReturnPlan.path = pathToHub;
                    returnPlan.push_back(newReturnPlan);
                } else {        //il trimitem la cea mai apropiata statie din drum spre hub
                    std::vector<std::pair<int, int>> pathToStation = nearestStationToHub(map, *agent, agent->getMapPos());
                    if (pathToStation.empty()) {
                        //cumva agentul nu are cum sa se intoarca desi a fost trimis doar daca are cum (nu ar tb sa se intample asta)
                    } else {
                        Route newReturnPlan;
                        newReturnPlan.agentIdx = agentIdx;
                        newReturnPlan.path = pathToStation;
                        returnPlan.push_back(newReturnPlan);
                    }
                }
            }
        }
    }

    return returnPlan;
}