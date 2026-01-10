#include "map/map.h"
#include "map/genesis.h"
#include "map/file_map_loader.h"
#include "map/procedural_map.h"
#include "map/config.h"
#include "simulation/simulation.h"

#include <iostream>
#include <memory>
#include <exception>
#include <limits>

static void waitForEnter() {
    std::cout << "\nPress ENTER to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main() {
    try {
        std::cout << "=========================================\n";
        std::cout << " HiveMind - Autonomous Logistics Simulator\n";
        std::cout << "=========================================\n\n";

        // 1. Load configuration (Singleton)
        Config& cfg = Config::getInstance();

        std::cout << "[MAIN] Configuration loaded\n";
        std::cout << "       Map size: "
                  << cfg.getMapSize().first << "x"
                  << cfg.getMapSize().second << "\n";
        std::cout << "       Max ticks: "
                  << cfg.getMaxTicks() << "\n\n";

        // 2. Choose map generation strategy
        std::unique_ptr<IMapGenerator> mapGenerator;
        int choice = 0;

        std::cout << "Choose map generation mode:\n";
        std::cout << "  1) Load map from file (debug / testing)\n";
        std::cout << "  2) Procedural random map (default)\n";
        std::cout << "\nYour choice: ";
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (choice == 1) {
            mapGenerator = std::make_unique<FileMapLoader>();
            std::cout << "\n[MAIN] FileMapLoader selected\n";
        } else {
            mapGenerator = std::make_unique<ProceduralMapGenerator>();
            std::cout << "\n[MAIN] ProceduralMapGenerator selected\n";
        }

        // 3. Generate map
        std::cout << "[MAIN] Generating map...\n";
        Map map = mapGenerator->generate();
        std::cout << "[MAIN] Map generated successfully\n";

        waitForEnter();

        // 4. Initialize simulation
        std::cout << "\n[MAIN] Initializing simulation...\n";
        Simulation simulation(map, cfg);

        std::cout << "[MAIN] Simulation ready\n";
        std::cout << "[INFO] Agents will spawn in HUB\n";
        std::cout << "[INFO] Packages will be generated dynamically\n";

        waitForEnter();

        // 5. Run simulation
        std::cout << "\n[MAIN] Starting simulation loop...\n\n";
        simulation.run(cfg);

    } catch (const std::exception& e) {
        std::cerr << "\n[CRITICAL ERROR] " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "\n[CRITICAL ERROR] Unknown exception occurred\n";
        return 1;
    }

    std::cout << "\n[MAIN] Simulation finished successfully\n";
    return 0;
}
