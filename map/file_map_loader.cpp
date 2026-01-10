#include "file_map_loader.h"
#include "config.h"

#include <fstream>
#include <string>
#include <vector>

Map FileMapLoader::generate () {
    Config& configs = Config::getInstance();
    std::vector<std::vector<CellType>> newGrid;

    std::pair<int, int> newMapSize = {-1, 0};
    int newMaxStations = 0;
    int newClientsCount = 0;

    std::ifstream mapFile("map/preloaded_map.txt");

    if(!mapFile.is_open())
        throw std::runtime_error("Eroare: fisierul cu harta nu a putut fi deschis!");

    std::string line;
    while(std::getline(mapFile, line)) {
        if(newMapSize.first == -1)
            newMapSize.first = line.size();
        else {
            if(newMapSize.first != line.size())
                throw std::runtime_error("Eroare: harta nu este dreptunghica!");
        }

        std::vector<CellType> newGridLine;
        for(char x : line) {
            switch (x) {
                case '.':
                    newGridLine.push_back(CellType::ROAD);
                    break;
                case '#':
                    newGridLine.push_back(CellType::WALL);
                    break;
                case 'B':
                    newGridLine.push_back(CellType::HUB);   //verificam daca harta are HUB in validateMap()
                    break;
                case 'D':
                    newGridLine.push_back(CellType::DESTINATION);
                    newClientsCount++;
                    break;
                case 'S':
                    newGridLine.push_back(CellType::STATION);;
                    newMaxStations++;
                    break;
                default:
                    throw std::runtime_error("Eroare: harta nu este dreptunghica / are caractere invalide!");
            }
        }
        
        newGrid.push_back(newGridLine);
        newMapSize.second++;
    }
    mapFile.close();

    Map map(newMapSize);
    map.loadMap(newGrid);
    if(configs.getRobots() + configs.getScooters() > 0 && !validateMap(map))
        throw std::runtime_error("Eroare: harta nu este valida");

    configs.setMapSize(newMapSize);
    configs.setMaxStations(newMaxStations);
    configs.setClientsCount(newClientsCount);

    return map;
}