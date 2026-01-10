#include "agent.h"

Agent::Agent (int newSpeed, int newMaxBattery, int newBatteryPerTick, int newCostPerTick, int newTotalPackageCapacity, std::pair<int, int> newMapPos) :
    speed(newSpeed), maxBattery(newMaxBattery), batteryPerTick(newBatteryPerTick), costPerTick(newCostPerTick),
    totalPackageCapacity(newTotalPackageCapacity), mapPos(newMapPos)
{}

bool Agent::isAlive() const {
    return state != AgentState::DEAD;
};

int Agent::getSpeed() const { return speed; };
int Agent::getBatteryPerTick () const { return batteryPerTick; };
int Agent::getCurrentBattery () const { return currentBattery; };
int Agent::getCostPerTick () const { return costPerTick; };
int Agent::getTotalCarriedPackages() const { return carriedPackages.size(); };
int Agent::getTotalPackageCapacity() const { return totalPackageCapacity; };
bool Agent::hasActivePath() const { return !currentPath.empty(); };
bool Agent::hasPackages() const { return !carriedPackages.empty(); };
std::pair<int, int> Agent::getMapPos() const { return mapPos; };
int Agent::getTotalCost() const { return totalCost; };

AgentState Agent::getState() const {
    return state;
};

void Agent::moveAgent(std::pair<int, int> newPos)  {
    mapPos = newPos;
};

void Agent::deliverPackage() {
    if(!hasPackages())
        return;

    std::shared_ptr<Package> p = carriedPackages.front();
    carriedPackages.erase(carriedPackages.begin());

    p->markDelivered();

    currentPath.clear();
    pathIdx = 0;


    if (carriedPackages.empty()) {
        state = AgentState::IDLE;
    } else {

        //pt mai multe pachete

        state = AgentState::IDLE;
    }
};

void Agent::tick(CellType cellType) {
    if (state == AgentState::DEAD)
        return;

    if (currentBattery <= 0) {
        state = AgentState::DEAD;
        return;
    }

    if (state == AgentState::MOVING) {
        for (int i = 0; i < speed; i++) {
            if(pathIdx >= currentPath.size()) {
                
                if (hasPackages())
                    deliverPackage();
                else
                    state = AgentState::IDLE;

                return;
            }

            moveAgent(currentPath[pathIdx]);
            pathIdx++;
        }

        totalCost += costPerTick;

        currentBattery -= batteryPerTick;
        if (currentBattery <= 0) {
            state = AgentState::DEAD;
            return;
        }
    }

    if(state == AgentState::IDLE) {
        if(cellType == CellType::HUB || cellType == CellType::STATION)
            state = AgentState::CHARGING;
    }

    if (state == AgentState::CHARGING) {
        currentBattery += (maxBattery * 25) / 100;
        if(currentBattery >= maxBattery) {
            currentBattery = maxBattery;
            state = AgentState::IDLE;
        }
    }
}

bool Agent::isIdle() const { return AgentState::IDLE == state; };

void Agent::assignPackage(std::shared_ptr<Package> newPackage) {
    if (carriedPackages.size() >= totalPackageCapacity)
        return;

    carriedPackages.push_back(newPackage);
    newPackage->markInTransit();
};

void Agent::assignPath (const std::vector<std::pair<int, int>>& path) {
    if(!isIdle())
        return;

    currentPath = path;
    pathIdx = 0;
    state = AgentState::MOVING;
}