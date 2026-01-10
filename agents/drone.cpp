#include "drone.h"
#include "../map/map.h"

#include <utility>

Drone::Drone(std::pair<int, int> newMapPos) : Agent (3, 100, 10, 15, 1, newMapPos) {
    currentBattery = 100;
}

bool Drone::canTraverse (CellType type) const { return true; }
char Drone::getSymbol() const { return '^'; }