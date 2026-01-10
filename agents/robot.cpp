#include "robot.h"
#include "../map/map.h"

#include <utility>

Robot::Robot(std::pair<int, int> newMapPos) : Agent (1, 300, 2, 1, 4, newMapPos) {
    currentBattery = 300;
}

bool Robot::canTraverse (CellType type) const { return CellType::WALL != type; }
char Robot::getSymbol() const  { return 'R'; }