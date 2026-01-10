#include "simulation.h"
#include "../agents/agent_factory.h"
#include "../orders/package_manager.h"
#include "../navigation/navigation.h"

#include <chrono>
#include <thread>
#include <utility>
#include <iostream>
#include <algorithm>
#include <fstream>

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
            if (currentTick > p->getSpawnTick() + p->getDeadline()) {
                totalProfit -= 50;
                lateCount++;
            } else {
                totalProfit += p->getReward();
                deliveredCount++;
            }

        }
    }

    // Ștergem tot ce nu mai este WAITING sau IN_TRANSIT
    packages.erase(
        std::remove_if(packages.begin(), packages.end(),
            [](const std::shared_ptr<Package>& p) {
                return p->getStatus() == PackageStatus::DELIVERED || 
                       p->getStatus() == PackageStatus::EXPIRED;
            }
        ),
        packages.end()
    );
}

void Simulation::spawnAgentFleet(const Config& configs, std::pair<int, int> hub) { 

    for (int i = 0; i < configs.getDrones(); i++)
        agents.push_back(AgentFactory::createAgent(AgentType::DRONE, hub));
    for (int i = 0; i < configs.getRobots(); i++)
        agents.push_back(AgentFactory::createAgent(AgentType::ROBOT, hub));
    for (int i = 0; i < configs.getScooters(); i++)
        agents.push_back(AgentFactory::createAgent(AgentType::SCOOTER, hub));
};

void Simulation::updateAgents (std::vector<Route> deliveryPlans, std::vector<Route> returnPlans) {

    for (auto plan : deliveryPlans)
        if(plan.agentIdx != -1 && plan.packageIdx != -1) {
            agents[plan.agentIdx]->assignPackage(packages[plan.packageIdx]);
            agents[plan.agentIdx]->assignPath(plan.path);
        }
    
    for (auto plan : returnPlans)
        agents[plan.agentIdx]->assignPath(plan.path);

    for (auto& agent : agents)
        agent->tick(map.getCell(agent->getMapPos()));
}

void Simulation::run (const Config& configs) {

    std::random_device rd;
    std::mt19937 gen(rd());

    std::pair<int, int> hub = map.getHubPosition();

    spawnAgentFleet(configs, hub);        

    for (currentTick = 0; currentTick < maxTicks; currentTick++) {
        spawnPackagesIfNeeded (gen, map.getDestinations());
        
        updatePackages();
        
        std::vector<Route> deliveryPlans = hiveMind.decideDelivery (map, agents, packages, currentTick);
        std::vector<Route> returnPlans = hiveMind.decideReturn (map, agents);
        
        updateAgents(deliveryPlans, returnPlans);

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

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
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

    for(auto& agent : agents)
        if (agent->getState() == AgentState::DEAD)  deadAgents++;

    std::ofstream reportFile("simulation.txt");
    
    if (reportFile.is_open()) {
        reportFile << "=========================================\n";
        reportFile << "   HIVEMIND SIMULATION REPORT\n";
        reportFile << "=========================================\n\n";
        
        reportFile << "CONFIGURATION:\n";
        reportFile << "  Map Size: " << configs.getMapSize().first << "x" << configs.getMapSize().second << "\n";
        reportFile << "  Total Ticks: " << maxTicks << "\n";
        reportFile << "  Fleet Composition:\n";
        reportFile << "    - Drones: " << configs.getDrones() << "\n";
        reportFile << "    - Robots: " << configs.getRobots() << "\n";
        reportFile << "    - Scooters: " << configs.getScooters() << "\n\n";
        
        reportFile << "SIMULATION RESULTS:\n";
        reportFile << "  Total Packages: " << totalPackages << "\n";
        reportFile << "  Delivered on Time: " << deliveredCount << "\n";
        reportFile << "  Delivered Late: " << lateCount << "\n";
        reportFile << "  Undelivered: " << unDeliveredCount << "\n";
        reportFile << "  Success Rate: " << (succesRate * 100) << "%\n\n";
        
        reportFile << "FLEET STATUS:\n";
        reportFile << "  Dead Agents: " << deadAgents << "\n";
        reportFile << "  Surviving Agents: " << (agents.size() - deadAgents) << "\n\n";
        
        reportFile << "FINANCIAL SUMMARY:\n";
        reportFile << "  Total Profit: " << totalProfit << " credits\n";
        
        if (totalProfit > 0)
            reportFile << "  Status: PROFITABLE :D\n";
        else
            reportFile << "  Status: LOSS :(\n";
        
        reportFile << "\n=========================================\n";
        reportFile << "  End of Report\n";
        reportFile << "=========================================\n";
        
        reportFile.close();
        
        std::cout << "\n[INFO] Simulation report saved to 'simulation.txt'\n";
    } else {
        std::cerr << "\n[ERROR] Could not create simulation.txt file\n";
    }

    std::cout << "\n=== SIMULATION ENDED ===" << std::endl;
    std::cout << "Total packages: " << totalPackages << std::endl;
    std::cout << "Total profit: " << totalProfit << std::endl;
    std::cout << "Total delivered in time: " << deliveredCount << std::endl;
    std::cout << "Total delivered late: " << lateCount << std::endl;
    std::cout << "Package delivered success rate: " << succesRate << std::endl;
    std::cout << "Dead agents: " << deadAgents << std::endl;
}