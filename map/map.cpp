#include "map.h"

#include <iostream>
#include <vector>
#include <utility>

using namespace std;

Map::Map (const pair<int, int>& mapSize) {
    size.first = mapSize.first;
    size.second = mapSize.second;
    grid.assign(size.first, vector<CellType>(size.second, CellType::ROAD));
}

void Map::resetMap() {
    grid.assign(size.first, vector<CellType>(size.second, CellType::ROAD));
}

void Map::loadMap (std::vector<std::vector<CellType>> newGrid) {
    grid = newGrid;
    
    for(int i = 0; i < size.first; i++)
        for(int j = 0; j < size.second; j++) {
            switch (grid[i][j]) {
                case CellType::HUB:
                    hubPosition = {i, j};
                    break;
                case CellType::DESTINATION:
                    destinations.push_back({i, j});
                    break;
                case CellType::STATION:
                    stations.push_back({i, j});
                    break;
                default:
                    throw out_of_range("Error: No cell type");
                    break;
            }
        }
}

bool Map::inBounds (const pair<int, int>& pos) const {
    return pos.first >= 0 && pos.first < size.first &&
           pos.second >= 0 && pos.second < size.second;
}

CellType Map::getCell(const pair<int, int>& pos) const {
    if (!inBounds(pos))
        throw out_of_range("Map::getCell out of bounds");
    return grid[pos.first][pos.second];
}

std::pair<int, int> Map::getSize() const { return size; };

void Map::setCell (const pair<int, int>& pos, CellType type) {
    if (!inBounds(pos))
        throw out_of_range("Map::getCell out of bounds");
    grid[pos.first][pos.second] = type;

    switch (type) {
        case CellType::HUB:
            hubPosition = pos;
            break;
        case CellType::DESTINATION:
            destinations.push_back(pos);
            break;
        case CellType::STATION:
            stations.push_back(pos);
            break;
        case CellType::ROAD:
            break;
        case CellType::WALL:
            break;
        default:
            throw out_of_range("Error: No cell type");
            break;
    }
}

bool Map::isWalkable (const pair<int, int>& pos) const {
    return (grid[pos.first][pos.second] == CellType::ROAD ||
    grid[pos.first][pos.second] == CellType::HUB ||
    grid[pos.first][pos.second] == CellType::STATION ||
    grid[pos.first][pos.second] == CellType::DESTINATION);
}

char Map::toChar (CellType type) const {
    switch (type) {
        case CellType::ROAD:
            return '.';
        case CellType::WALL:
            return '#';
        case CellType::HUB:
            return 'B';
        case CellType::STATION:
            return 'S';
        case CellType::DESTINATION:
            return 'D';
        default:
            return '?';
    }
}

std::pair<int, int> Map::getHubPosition() const { return hubPosition; };
const std::vector<std::pair<int, int>>& Map::getDestinations() const { return destinations; };
const std::vector<std::pair<int, int>>& Map::getStations() const { return stations; };