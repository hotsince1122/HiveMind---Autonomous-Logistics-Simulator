#include "scooter.h"
#include "../map/map.h"

Scooter::Scooter(std::pair<int, int> newMapPos) : Agent (2, 200, 5, 4, 2, newMapPos) {
    currentBattery = 200;
}

bool Scooter::canTraverse (CellType type) const { return CellType::WALL != type; }

char Scooter::getSymbol() const { return 'M'; };        // M de la motor, S e deja ocupat de statii