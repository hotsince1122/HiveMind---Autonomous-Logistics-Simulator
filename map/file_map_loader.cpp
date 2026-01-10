#include "file_map_loader.h"
#include "config.h"

#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

Map FileMapLoader::generate() {
    Config& configs = Config::getInstance();
    
    std::vector<std::vector<CellType>> rows; 
    std::pair<int, int> detectedSize = {0, 0};
    int newMaxStations = 0;
    int newClientsCount = 0;

    std::ifstream mapFile("map/preloaded_map.txt");

    if(!mapFile.is_open())
        throw std::runtime_error("Eroare: fisierul cu harta nu a putut fi deschis!");

    std::string line;
    while(std::getline(mapFile, line)) {
        if(line.empty()) continue;
        
        bool hasContent = false;
        for(char c : line) if(!isspace(c)) hasContent = true;
        if(!hasContent) continue;

        std::vector<CellType> currentRow;
        int currentWidth = 0;

        for(char x : line) {
            if(x == ' ' || x == '\r' || x == '\t' || x == '\n')
                continue;

            currentWidth++;

            switch (x) {
                case '.': currentRow.push_back(CellType::ROAD); break;
                case '#': currentRow.push_back(CellType::WALL); break;
                case 'B': currentRow.push_back(CellType::HUB); break;
                case 'D': 
                    currentRow.push_back(CellType::DESTINATION); 
                    newClientsCount++;
                    break;
                case 'S': 
                    currentRow.push_back(CellType::STATION); 
                    newMaxStations++;
                    break;
                default:
                    throw std::runtime_error("Eroare: caracter invalid in harta!");
            }
        }

        if(detectedSize.first == 0)
            detectedSize.first = currentWidth;
        else if(detectedSize.first != currentWidth)
            throw std::runtime_error("Eroare: harta nu este dreptunghica!");

        rows.push_back(currentRow);
        detectedSize.second++;
    }
    mapFile.close();

    if(detectedSize.first == 0 || detectedSize.second == 0)
        throw std::runtime_error("Eroare: fisierul hartii este gol!");

    //transpunere
    std::vector<std::vector<CellType>> finalGrid(detectedSize.first, std::vector<CellType>(detectedSize.second));

    for(int y = 0; y < detectedSize.second; y++) {
        for(int x = 0; x < detectedSize.first; x++) {
            finalGrid[x][y] = rows[y][x];
        }
    }

    Map map(detectedSize);
    map.loadMap(finalGrid);

    configs.setMapSize(detectedSize);
    configs.setMaxStations(newMaxStations);
    configs.setClientsCount(newClientsCount);

    if(configs.getRobots() + configs.getScooters() > 0 && !validateMap(map))
        throw std::runtime_error("Eroare: harta nu este valida (nu exista drum catre tinte)");

    return map;
}