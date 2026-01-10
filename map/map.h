#pragma once

#include <vector>
#include <utility>

enum class CellType {
    ROAD,   //Space
    WALL,
    HUB,
    STATION,
    DESTINATION
};

class Map {

    std::pair<int, int> size; 
    std::vector<std::vector<CellType>> grid;

    std::pair<int, int> hubPosition;
    std::vector<std::pair<int, int>> destinations;
    std::vector<std::pair<int, int>> stations;

    public:
        Map (const std::pair<int, int>& mapSize);

        void resetMap ();

        void loadMap (std::vector<std::vector<CellType>> newGrid);

        bool inBounds (const std::pair<int, int>& pos) const;

        CellType getCell(const std::pair<int, int>& pos) const;
        std::pair<int, int> getSize() const;

        void setCell (const std::pair<int, int>& pos, CellType type);

        bool isWalkable (const std::pair<int, int>& pos) const;

        char toChar (CellType type) const;

        std::pair<int, int> getHubPosition() const;
        const std::vector<std::pair<int, int>>& getDestinations() const;
        const std::vector<std::pair<int, int>>& getStations() const;
        
};