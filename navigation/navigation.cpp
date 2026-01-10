#include "navigation.h"
#include "../map/config.h"
#include <queue>
#include <algorithm> // pentru std::reverse

using namespace std;

vector<pair<int, int>> Navigation::getPath(const Map& map, pair<int, int> start, pair<int, int> end, const Agent& agent) {
    
    // 1. Verificari de baza
    if (!map.inBounds(start) || !map.inBounds(end))
        return {}; // Coordonate invalide

    if (start == end)
        return {}; // Suntem deja la destinatie

    // Daca destinatia e un zid si agentul NU zboara, nu are rost sa cautam
    // Nota: Asta e o optimizare, algoritmul oricum ar returna {}
    if (!agent.canTraverse(map.getCell(end))) 
        return {};

    auto size = Config::getInstance().getMapSize();

    // 2. Matricea de parinti pentru reconstruirea drumului
    // parents[x][y] retine coordonata celulei din care am venit ca sa ajungem in (x,y)
    // Initializam cu {-1, -1} care inseamna "nevizitat"
    vector<vector<pair<int, int>>> parents(size.first, vector<pair<int, int>>(size.second, {-1, -1}));

    // Coada pentru BFS
    queue<pair<int, int>> q;

    // Adaugam startul
    q.push(start);
    parents[start.first][start.second] = {-2, -2}; // Marcam startul ca vizitat cu o valoare speciala

    // Directii de miscare (Sus, Jos, Stanga, Dreapta)
    int dx[] = {0, 0, 1, -1};
    int dy[] = {1, -1, 0, 0};

    bool found = false;

    // 3. Executia BFS
    while (!q.empty()) {
        pair<int, int> current = q.front();
        q.pop();

        // Daca am ajuns la destinatie, ne oprim
        if (current == end) {
            found = true;
            break;
        }

        // Exploram vecinii
        for (int i = 0; i < 4; i++) {
            int nx = current.first + dx[i];
            int ny = current.second + dy[i];
            pair<int, int> neighbor = {nx, ny};

            // Verificam: 
            // 1. Daca e in harta
            // 2. Daca NU a fost vizitat (parents e inca -1,-1)
            // 3. Daca agentul POATE intra fizic in celula (folosind metoda polimorfica)
            if (map.inBounds(neighbor) && 
                parents[nx][ny].first == -1 && parents[nx][ny].second == -1 && 
                agent.canTraverse(map.getCell(neighbor))) {
                
                // Setam parintele si adaugam in coada
                parents[nx][ny] = current;
                q.push(neighbor);
            }
        }
    }

    // 4. Reconstructia drumului (Backtracking)
    vector<pair<int, int>> path;

    if (found) {
        pair<int, int> curr = end;
        
        // Mergem din parinte in parinte pana ajungem la start
        // Conditia de oprire: parintele e {-2, -2} (adica startul)
        while (curr != start) {
            path.push_back(curr);
            curr = parents[curr.first][curr.second];
        }

        // Drumul acum este invers (End -> ... -> Primul Pas), deci il intoarcem
        std::reverse(path.begin(), path.end());
    }

    return path;
}