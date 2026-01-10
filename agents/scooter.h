#include "agent.h"
#include "../map/map.h"


class Scooter : public Agent {

    public:
        Scooter(std::pair<int, int> newMapPos);

        bool canTraverse (CellType type) const override;
        char getSymbol() const override;
};