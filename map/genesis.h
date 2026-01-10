#pragma once

#include "map.h"
#include "config.h"

class IMapGenerator {
    protected:
        bool validateMap (const Map& map);

    public:
        virtual Map generate() = 0;
        virtual ~IMapGenerator() {}
};