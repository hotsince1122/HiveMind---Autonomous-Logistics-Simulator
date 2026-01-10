#include "package_manager.h"

Package PackageManager::packageDepolyer (int currentTick, const std::vector<std::pair<int, int>>& destinations, std::mt19937& gen) {

    std::uniform_int_distribution<> idx(0, destinations.size() - 1);
    int randomIndex = idx(gen);

    std::pair<int, int> newDestination = destinations[randomIndex];

    std::uniform_int_distribution<> rwrd(200, 800);
    int newReward = rwrd(gen);

    std::uniform_int_distribution<> ded(10, 20);
    int newDeadline = ded(gen);

    Package newPackage (newDestination, newReward, newDeadline, currentTick);

    return newPackage;
}
