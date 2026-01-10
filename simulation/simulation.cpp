#include "simulation.h"
#include "../agents/agent.h"
#include "../agents/drone.h"
#include "../agents/robot.h"
#include "../agents/scooter.h"
#include "../orders/package_manager.h"
#include "../navigation/navigation.h"

#include <chrono>
#include <thread>
#include <utility>
#include <iostream>

Simulation::Simulation (Map& newMap, const Config& configs) : map(newMap) {
    maxTicks = configs.getMaxTicks();
    totalPackages = configs.getTotalPackages();
    spawnFreq = configs.getSpawnFreq();
}

void Simulation::spawnPackagesIfNeeded (std::mt19937& gen, const std::vector<std::pair<int, int>>& destinations) {

    if(currentTick % spawnFreq == 0 && spawnedPackages < totalPackages) {
        spawnedPackages++;
        packages.push_back(std::make_shared<Package>(PackageManager::packageDepolyer(currentTick, destinations, gen)));
        return;
    }
    return;
};

void Simulation::updatePackages() {
    for (auto& p : packages) {
        if (p->getStatus() == PackageStatus::DELIVERED) {
            // Verificăm dacă a fost întârziat (fără să stricăm statusul DELIVERED încă)
            // Presupunem că ai o metodă care doar verifică timpul, fără side-effects
            if (currentTick > p->getSpawnTick() + p->getDeadline()) {
                totalProfit -= 50;
                lateCount++;
            } else {
                totalProfit += p->getReward();
                deliveredCount++;
            }
            
            // Îl marcăm pentru ștergere (folosim un status special sau 
            // ne asigurăm că remove_if îl vede)
        }
    }

    // Ștergem tot ce nu mai este WAITING sau IN_TRANSIT
    packages.erase(
        std::remove_if(packages.begin(), packages.end(),
            [](const std::shared_ptr<Package>& p) {
                return p->getStatus() == PackageStatus::DELIVERED || 
                       p->getStatus() == PackageStatus::EXPIRED; // <--- Șterge și resturile
            }
        ),
        packages.end()
    );
}

void Simulation::spawnAgentFleet(const Config& configs, std::pair<int, int> hub) { 

    for (int i = 0; i < configs.getDrones(); i++)
        agents.push_back(std::make_unique<Drone>(hub));
    for (int i = 0; i < configs.getRobots(); i++)
        agents.push_back(std::make_unique<Robot>(hub));
    for (int i = 0; i < configs.getScooters(); i++)
        agents.push_back(std::make_unique<Scooter>(hub));
};

void Simulation::updateAgents (std::vector<DeliveryPlan> deliveryPlans) {

    for (auto plan : deliveryPlans)
        if(plan.agentIdx != -1 && plan.packageIdx != -1) {
            agents[plan.agentIdx]->assignPackage(packages[plan.packageIdx]);
            agents[plan.agentIdx]->assignPath(plan.path);
        }

    for (auto& a : agents) {
        if(!a->isAlive())
            continue;

        CellType currentCell = map.getCell(a->getMapPos());
        bool onChargingSpot = (currentCell == CellType::HUB || currentCell == CellType::STATION);
        
        if(a->isIdle() && !a->hasPackages() && !onChargingSpot) {

            std::vector<std::pair<int, int>> safeSpots = map.getStations();
            safeSpots.push_back(map.getHubPosition());

            std::vector<std::pair<int, int>> bestPath;
            int minPathSize = 9999999;

            for (const auto& spot : safeSpots) {
                auto path = Navigation::getPath(map, a->getMapPos(), spot, *a);
                if (!path.empty() && (int)path.size() < minPathSize) {
                    minPathSize = path.size();
                    bestPath = path;
                }
            }

            if (!bestPath.empty())
                a->assignPath(bestPath);
        }

        a->tick(map.getCell(a->getMapPos()));

    }
}

void Simulation::run (const Config& configs) {

    std::random_device rd;
    std::mt19937 gen(rd());

    std::pair<int, int> hub = map.getHubPosition();

    spawnAgentFleet(configs, hub);        

    for (currentTick = 0; currentTick < maxTicks; currentTick++) {
        spawnPackagesIfNeeded (gen, map.getDestinations());
        
        updatePackages();
        
        std::vector<DeliveryPlan> deliveryPlans = hiveMind.decide (map, agents, packages, currentTick);

        updateAgents(deliveryPlans);

        //---Aici afisam statistici live + harta cu agentii pe ea---
        std::cout << "\033[2J\033[1;1H";

        std::cout << "=== HIVE MIND SIMULATION ===" << std::endl;
        std::cout << "Tick: " << currentTick << " / " << maxTicks << std::endl;
        std::cout << "Profit: " << totalProfit << " | Active Packages: " << packages.size() << std::endl;
        std::cout << "Delivered: " << deliveredCount << " | Late: " << lateCount << std::endl;
        std::cout << "----------------------------" << std::endl;

        std::vector<std::string> displayGrid(map.getSize().second, std::string(map.getSize().first, ' '));

        //harta fara agenti pe ea
        for(int y = 0; y < map.getSize().second; y++) {
            for(int x = 0; x < map.getSize().first; x++) {
                displayGrid[y][x] = map.toChar(map.getCell({x, y}));
            }
        }

        for(const auto& agent : agents) {
            if(!agent->isAlive()) continue;

            auto pos = agent->getMapPos();
            
            // safety check pentru limite
            if(pos.first < 0 || pos.first >= map.getSize().first || pos.second < 0 || pos.second >= map.getSize().second)
                continue;

            if (pos == map.getHubPosition())        //Hub este prioritar la afisare 
                continue;

            char agentChar = agent->getSymbol();

            // Verificam coliziunea vizuala (daca e deja alt agent acolo desenam *)
            char mapChar = map.toChar(map.getCell(pos));
            char currentChar = displayGrid[pos.second][pos.first];

            if(currentChar != mapChar) {
                displayGrid[pos.second][pos.first] = '*'; // Mai multi agenti in acelasi loc
            } else {
                displayGrid[pos.second][pos.first] = agentChar;
            }
        }

        //afiam grid-ul final
        for(int i = 0; i < map.getSize().second; i++) {
            for(int j = 0; j < map.getSize().first; j++)
                std::cout << displayGrid[i][j] << ' ';
            std::cout << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    for (auto& p : packages) {
        if (p->isExpired(currentTick)) {
            totalProfit -= 200;
            unDeliveredCount++;
        }
    }

    for (auto& a : agents) {
        if(!a->isAlive())
            totalProfit -= 500;

        totalProfit -= a->getTotalCost();       //scadem mentenanta indiferent daca agentul moare sau nu
    }

    double succesRate = (totalPackages > 0) ? deliveredCount / static_cast<double> (totalPackages) : 0.0;

    std::cout << "\n=== SIMULATION ENDED ===" << std::endl;
    std::cout << "Total packages: " << totalPackages << std::endl;
    std::cout << "Total profit: " << totalProfit << std::endl;
    std::cout << "Total delivered in time: " << deliveredCount << std::endl;
    std::cout << "Total delivered late: " << lateCount << std::endl;
    std::cout << "Package delivered success rate: " << succesRate << std::endl;
}