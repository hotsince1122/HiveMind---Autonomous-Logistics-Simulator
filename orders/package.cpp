#include "package.h"

#include <utility>

Package::Package (std::pair<int, int> newDestination, int newReward, int newDeadline, int newSpawnTick) :
                  destination(newDestination),  reward(newReward), deadline(newDeadline), spawnTick(newSpawnTick)  {}

bool Package::isExpired(int currentTick) {
    if(currentTick > spawnTick + deadline) {
        status = PackageStatus::EXPIRED;
        return true;
    }
    return false;
}

int Package::getDeadline () const { return deadline; };

PackageStatus Package::getStatus() const { return status; };
std::pair<int, int> Package::getDestiation() const { return destination; };
int Package::getReward() const { return reward; };
int Package::getSpawnTick() const { return spawnTick; };

void Package::markInTransit() { status = PackageStatus::IN_TRANSIT; };
void Package::markDelivered() { status = PackageStatus::DELIVERED; };
void Package::markExpired() { status = PackageStatus::EXPIRED; };