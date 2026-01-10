#pragma once

#include <utility>

class Config {

    //--- mediu ---
    std::pair<int, int> mapSize;
    int maxStations;
    int ClientsCount;

    //--- flota ---
    int drones;
    int robots;
    int scooters;

    //--- simulare ---
    int maxTicks;
    int totalPackages;
    int spawnFreq;

    Config();
    void validateConfig() const;

    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    public:
        static Config& getInstance();

        std::pair<int, int> getMapSize() const;
        int getMaxStations() const;
        int getClientsCount() const;

        void setMapSize(const std::pair<int, int>& newMapSize);
        void setMaxStations(int newMaxStations);
        void setClientsCount(int newClientCount);

        int getDrones() const;
        int getRobots() const;
        int getScooters() const;

        int getMaxTicks() const;
        int getTotalPackages() const;
        int getSpawnFreq() const;
};
