#pragma once

#include "agent.h"
#include "drone.h"
#include "robot.h"
#include "scooter.h"
#include <memory>
#include <string>
#include <stdexcept>

enum class AgentType {
    DRONE,
    ROBOT,
    SCOOTER
};

class AgentFactory {
public:
    static std::unique_ptr<Agent> createAgent(AgentType type, std::pair<int, int> spawnPos) {
        switch (type) {
            case AgentType::DRONE:
                return std::make_unique<Drone>(spawnPos);
            
            case AgentType::ROBOT:
                return std::make_unique<Robot>(spawnPos);
            
            case AgentType::SCOOTER:
                return std::make_unique<Scooter>(spawnPos);
            
            default:
                throw std::invalid_argument("Unknown agent type");
        }
    }
    
    static std::unique_ptr<Agent> createAgent(const std::string& typeName, std::pair<int, int> spawnPos) {
        if (typeName == "DRONE" || typeName == "drone")
            return createAgent(AgentType::DRONE, spawnPos);
        else if (typeName == "ROBOT" || typeName == "robot")
            return createAgent(AgentType::ROBOT, spawnPos);
        else if (typeName == "SCOOTER" || typeName == "scooter")
            return createAgent(AgentType::SCOOTER, spawnPos);
        else
            throw std::invalid_argument("Unknown agent type: " + typeName);
    }
};