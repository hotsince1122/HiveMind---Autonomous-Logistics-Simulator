#include "genesis.h"

#include <queue>
#include <vector>


bool IMapGenerator::validateMap(const Map& map) {
    auto size = Config::getInstance().getMapSize();
        
    std::pair<int, int> startPos = {-1, -1};
    int totalTargets = 0;

    for (int x = 0; x < size.first; x++) {
        for (int y = 0; y < size.second; y++) {
            CellType type = map.getCell({x, y});
            if (type == CellType::HUB) {
                startPos = {x, y};
            } else if (type == CellType::STATION || type == CellType::DESTINATION) {
                totalTargets++;
            }
        }
    }

    if (startPos.first == -1) return false;     //daca harta nu are HUB atunci e invalida
    if (totalTargets == 0) return true;     //daca harta nu are clienti atunci e triviala

    std::vector<std::vector<bool>> visited(size.first, std::vector<bool>(size.second, false));
    std::queue<std::pair<int, int>> q;

    q.push(startPos);
    visited[startPos.first][startPos.second] = true;

    int foundTargets = 0;

    int dx[] = {0, 0, 1, -1};
    int dy[] = {1, -1, 0, 0};

    while (!q.empty()) {
        auto current = q.front();
        q.pop();

        for (int i = 0; i < 4; i++) {
            int nx = current.first + dx[i];
            int ny = current.second + dy[i];
            std::pair<int, int> neighbor = {nx, ny};

            // berificam daca vecinul e valid
            if (map.inBounds(neighbor) && !visited[nx][ny]) {
                CellType type = map.getCell(neighbor);

                // putem merge pe orice NU e WALL
                if (type != CellType::WALL) {
                    visited[nx][ny] = true;
                    q.push(neighbor);

                    // daca am gasit o tinta, o numaram
                    if (type == CellType::STATION || type == CellType::DESTINATION) {
                        foundTargets++;
                    }
                }
            }
        }
    }

    return foundTargets == totalTargets;
}