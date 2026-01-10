#pragma once

#include <utility>

enum class PackageStatus {
    WAITING,
    IN_TRANSIT,
    DELIVERED,
    EXPIRED
};

class Package {
    std::pair<int, int> destination;
    int reward;
    int deadline;       //in pdf se sugereaza faptul ca deadline-ul este cate tick-uri ai pana trebuie livrat pachetul
    int spawnTick;
    PackageStatus status = PackageStatus::WAITING;

    public:
        Package (std::pair<int, int> newDestination, int newReward, int newDeadline, int newSpawnTick);

        bool isExpired(int currentTick);

        int getDeadline () const;
        int getReward() const;
        int getSpawnTick() const;

        PackageStatus getStatus() const;
        std::pair<int, int> getDestiation() const;

        void markInTransit();
        void markDelivered();
        void markExpired();
};