#include "map.h"
#include "config.h"
#include "procedural_map.h"

#include <iostream>
#include <utility>
#include <random>
#include <algorithm>
#include <queue>


int ProceduralMapGenerator::manhattanDistance (const std::pair<int, int>& a, const std::pair<int, int>& b) {
    return abs(a.first - b.first) + abs(a.second - b.second);
}

bool ProceduralMapGenerator::isPosValid (std::pair<int, int> newPos, const std::vector<std::pair<int, int>>& occupied, int minDistanceAllowed) {
    for (const auto& pos : occupied) {
        int dist = manhattanDistance(pos, newPos);
        if(dist < minDistanceAllowed)
            return false;
    }
    return true;
}

void ProceduralMapGenerator::placeWithSpacing (CellType type, int count, int maxRetries, int& minAllowedDistance,
    std::vector<std::pair<int, int>>& occupied, Map& map,
    std::uniform_int_distribution<>& rangeForWidth, std::uniform_int_distribution<>& rangeForHeight, std::mt19937& gen) {
    for(int i = 0; i < count; i++) {
        bool succes = false;
        for(int retry = 0; retry < maxRetries; retry++) {
            std::pair<int, int> newPos = {rangeForWidth(gen), rangeForHeight(gen)};
            if(isPosValid(newPos, occupied, minAllowedDistance)) {
                map.setCell(newPos, type);  
                occupied.push_back(newPos);
                succes = true;
                break;
            }
        }
        if (!succes) {
            if (minAllowedDistance > 0) {
                minAllowedDistance--;
                i--;        //reincercam sa plasam acelasi obiect cu distanta mai mica
            } else {
                throw std::runtime_error("Error: Cannot generate map!\n");
            }
        }
    }
}

void ProceduralMapGenerator::buildWALLs (Map& map, int maxRetries, int maxNrOfWALLs,
                                         std::uniform_int_distribution<>& rangeForWidth, std::uniform_int_distribution<>& rangeForHeight,
                                         std::mt19937& gen) {
    auto mapSize = Config::getInstance().getMapSize();
    std::uniform_int_distribution<> maxWALLSize(2, (((mapSize.first + mapSize.second) / 2) * 15) / 100);

    int WALLsBuild = 0;

    for(int i = 0; i < maxRetries && WALLsBuild < maxNrOfWALLs; i++) {
        std::pair<int, int> topLeftIdx = { rangeForWidth(gen), rangeForHeight(gen) };

        //verificam daca nu incepem pe vreun POI sau daca cladirea nu iese din bounds
        if (map.getCell(topLeftIdx) != CellType::ROAD)
            continue;
        std::pair<int, int> WALLSize = { maxWALLSize(gen), maxWALLSize(gen) };
        if(!map.inBounds({topLeftIdx.first + WALLSize.first - 1, topLeftIdx.second + WALLSize.second - 1}))
            continue;

        bool succes = true;
        for (int i = topLeftIdx.first; i < topLeftIdx.first + WALLSize.first; i++)
            for (int j = topLeftIdx.second; j < topLeftIdx.second + WALLSize.second; j++) {
                if (map.getCell({i, j}) != CellType::ROAD && map.getCell({i, j}) != CellType::WALL) {
                    succes = false;
                    break;
                }
                if(!succes) break;
            }
            
        if(succes == true) {
            for (int i = topLeftIdx.first; i < topLeftIdx.first + WALLSize.first; i++)
                for (int j = topLeftIdx.second; j < topLeftIdx.second + WALLSize.second; j++) {
                if (map.getCell({i, j}) != CellType::WALL) {
                    map.setCell({i, j}, CellType::WALL);
                    WALLsBuild++;
                }
            }
        }
    }
}

Map ProceduralMapGenerator::generate () {
    Config& configs = Config::getInstance();

    Map map (configs.getMapSize());

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> rangeForWidth(0, configs.getMapSize().first - 1);
    std::uniform_int_distribution<> rangeForHeight(0, configs.getMapSize().second - 1);

    bool succes;
    int globalAttempts = 0;
            
    do {
        // std::vector<std::pair<int, int>> freeCells;

        // for(int y = 0; y < configs.getMapSize().second; y++)
        //     for(int x = 0; x < configs.getMapSize().first; x++) {
        //         freeCells.push_back({x, y});
        //     }

        // std::shuffle(freeCells.begin(), freeCells.end(), gen);

        // int idx = 0;

        // auto HUBPos = freeCells[idx++];
        // map.setCell(HUBPos, CellType::HUB);

        // for(int i = 0; i < configs.getClientsCount(); i++) {
        //     auto clientPos = freeCells[idx++];
        //     map.setCell(clientPos, CellType::DESTINATION);
        // }

        // for(int i = 0; i < configs.getMaxSTATIONs(); i++) {
        //     auto STATIONPos = freeCells[idx++];
        //     map.setCell(STATIONPos, CellType::STATION);
        // }

        globalAttempts++;
        if (globalAttempts > 1000) {
            throw std::runtime_error("Eroare: Nu s-a putut genera o harta valida dupa 1000 de incercari.");
        }

        std::vector<std::pair<int, int>> occupied;

        int minAllowedDist = 3;
        int maxRetriesForPOIS = 67;

        try {
            placeWithSpacing(CellType::HUB, 1, maxRetriesForPOIS, minAllowedDist, occupied, map, rangeForWidth, rangeForHeight, gen);
            placeWithSpacing(CellType::DESTINATION, configs.getClientsCount(),
                             maxRetriesForPOIS, minAllowedDist, occupied, map, rangeForWidth, rangeForHeight, gen);
            placeWithSpacing(CellType::STATION, configs.getMaxStations(),
                             maxRetriesForPOIS, minAllowedDist, occupied, map, rangeForWidth, rangeForHeight, gen);
        } catch (const std::runtime_error& e) {
            continue;
        }

        int WALLDensity = 35;
        int maxRetriesForWALLs = 200;
        int maxNrOfWALLs = ((configs.getMapSize().first * configs.getMapSize().second) * WALLDensity ) / 100;

        buildWALLs(map, maxRetriesForWALLs, maxNrOfWALLs, rangeForWidth, rangeForHeight, gen);

        succes = validateMap(map);
        if (!succes)
            map.resetMap();
    } while(configs.getRobots() + configs.getScooters() > 0 && !succes);

    return map;
}