#pragma once

#include "map.h"
#include "genesis.h"

#include <utility>
#include <vector>
#include <random>

class ProceduralMapGenerator : public IMapGenerator {

    int manhattanDistance (const std::pair<int, int>& a, const std::pair<int, int>& b);

    bool isPosValid (std::pair<int, int> newPos, const std::vector<std::pair<int, int>>& occupied, int minDistanceAllowed);

    void placeWithSpacing (CellType type, int count, int maxRetries, int& minAllowedDistance,
        std::vector<std::pair<int, int>>& occupied, Map& map,
        std::uniform_int_distribution<>& rangeForWidth, std::uniform_int_distribution<>& rangeForHeight, std::mt19937& gen);

    void buildWALLs (Map& map, int maxRetries, int maxNrOfWALLs,
                     std::uniform_int_distribution<>& rangeForWidth, std::uniform_int_distribution<>& rangeForHeight, std::mt19937& gen);

    public:
        Map generate ();
};