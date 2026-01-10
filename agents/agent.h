#pragma once

#include <utility>
#include <memory>

#include "../map/map.h"
#include "../orders/package.h"

enum class AgentState {
    IDLE,
    MOVING,
    CHARGING,
    DEAD
};

class Agent {

    protected:
        //---agent characteristics---
        int speed;
        int maxBattery;
        int batteryPerTick;
        int costPerTick;
        int totalPackageCapacity;

        //---agent status---
        int currentBattery;
        std::pair<int, int> mapPos;
        AgentState state = AgentState::IDLE;
        std::vector<std::shared_ptr<Package>> carriedPackages;
        int pathIdx = 0;
        std::vector<std::pair<int, int>> currentPath;
        int totalCost = 0;


        Agent (int newSpeed, int newMaxBattery, int newBatteryPerTick, int newCostPerTick, int newTotalPackageCapacity, std::pair<int, int> newMapPos);

        
    public:
        virtual bool canTraverse (CellType cell) const = 0;
        virtual char getSymbol () const = 0;
        
        bool isAlive() const;
        bool isIdle() const;

        bool hasActivePath() const;
        bool hasPackages() const;
        
        AgentState getState() const;
        int getSpeed() const;
        int getBatteryPerTick () const;
        int getCurrentBattery () const;
        int getCostPerTick () const;
        int getTotalCarriedPackages() const;
        int getTotalPackageCapacity() const;
        std::pair<int, int> getMapPos() const;
        int getTotalCost() const;

        void tick(CellType cellType);

        void moveAgent(std::pair<int, int> newPos);
        void deliverPackage();
        
        void assignPath(const std::vector<std::pair<int, int>>& path);
        void assignPackage(std::shared_ptr<Package> newPackage);


        virtual ~Agent() {}
};