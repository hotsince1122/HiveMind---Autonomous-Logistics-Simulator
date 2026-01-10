#include "Config.h"

#include <fstream>
#include <string>
#include <map>
#include <functional>
#include <iostream>
#include <stdexcept>

Config::Config() {
    std::ifstream fin("map/simulation_setup.txt");

    if (!fin.is_open())
        throw std::runtime_error("Eroare la deschiderea fisierului simulation_setup.txt!");

    std::string label;

    std::map<std::string, std::function<void()>> handlers = {
        {"MAP_SIZE:",        [&]() { fin >> mapSize.first >> mapSize.second; }},
        {"MAX_TICKS:",       [&]() { fin >> maxTicks; }},
        {"MAX_STATIONS:",    [&]() { fin >> maxStations; }},
        {"CLIENTS_COUNT:",   [&]() { fin >> ClientsCount; }},
        {"DRONES:",          [&]() { fin >> drones; }},
        {"ROBOTS:",          [&]() { fin >> robots; }},
        {"SCOOTERS:",        [&]() { fin >> scooters; }},
        {"TOTAL_PACKAGES:",  [&]() { fin >> totalPackages; }},
        {"SPAWN_FREQUENCY:", [&]() { fin >> spawnFreq; }}
    };

    while (fin >> label) {
        if (handlers.count(label))
            handlers[label]();
        else
            std::cerr << "Eticheta necunoscuta: " << label << "\n";
    }

    fin.close();

    validateConfig();
}

void Config::validateConfig() const {
    if (mapSize.first <= 0 || mapSize.second <= 0)
        throw std::runtime_error("Dimensiunea hartii trebuie sa fie pozitiva");

    if (maxStations < 0)
        throw std::runtime_error("Numarul de statii nu poate fi negativ");

    if (ClientsCount <= 0)
        throw std::runtime_error("Trebuie cel putin un client");

    if (drones < 0 || robots < 0 || scooters < 0)
        throw std::runtime_error("Numar invalid de vehicule");

    if (drones + robots + scooters == 0)
        throw std::runtime_error("Flota este goala");

    if (maxTicks <= 0 || totalPackages <= 0 || spawnFreq <= 0)
        throw std::runtime_error("Parametri de simulare invalizi");
}

Config& Config::getInstance() {
    static Config instance;
    return instance;
}

std::pair<int, int> Config::getMapSize() const { return mapSize; }
int Config::getMaxStations() const { return maxStations; }
int Config::getClientsCount() const { return ClientsCount; }

void Config::setMapSize(const std::pair<int, int>& newMapSize) { mapSize = newMapSize; }
void Config::setMaxStations(int newMaxStations) { maxStations = newMaxStations; }
void Config::setClientsCount(int newClientsCount) { ClientsCount = newClientsCount; }

int Config::getDrones() const { return drones; }
int Config::getRobots() const { return robots; }
int Config::getScooters() const { return scooters; }

int Config::getMaxTicks() const { return maxTicks; }
int Config::getTotalPackages() const { return totalPackages; }
int Config::getSpawnFreq() const { return spawnFreq; }