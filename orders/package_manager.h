#pragma once

#include "package.h"

#include <vector>
#include <random>

class PackageManager {
    public:
        static Package packageDepolyer (int currentTick, const std::vector<std::pair<int, int>>& destinations, std::mt19937& gen);
};