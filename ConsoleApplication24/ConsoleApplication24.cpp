#include <iostream>
#include <vector>
#include <queue>
#include <cstring>
using namespace std;

#define N 6
#define INF INT_MAX

struct Node {
    vector<pair<int, int>> path;
    int reducedMatrix[N][N];
    int cost;
    int vertex;
    int level;
    
};

vector<double> theta;

Node* newNode(int parentMatrix[N][N],
    vector<pair<int, int>> const& path,
    int level, int i, int j)
{
    Node* node = new Node;
    node->path = path;

    if (level != 0) {
        node->path.push_back(make_pair(i, j));
    }

    memcpy(node->reducedMatrix, parentMatrix,
        sizeof node->reducedMatrix);

    // Удаляем строки и столбцы
    for (int k = 0; level != 0 && k < N; k++) {
        node->reducedMatrix[i][k] = INF;
        node->reducedMatrix[k][j] = INF;
    }

    node->reducedMatrix[j][0] = INF; // Удаляем начальную точку
    node->level = level;
    node->vertex = j;

    return node;
}

int rowReduction(int reducedMatrix[N][N], int row[N]) {
    fill_n(row, N, INF);

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (reducedMatrix[i][j] < row[i]) {
                row[i] = reducedMatrix[i][j];
            }
        }
    }

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (reducedMatrix[i][j] != INF && row[i] != INF) {
                reducedMatrix[i][j] -= row[i];
            }
        }
    }
    return 0;
}

int columnReduction(int reducedMatrix[N][N], int col[N]) {
    fill_n(col, N, INF);

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (reducedMatrix[i][j] < col[j]) {
                col[j] = reducedMatrix[i][j];
            }
        }
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (reducedMatrix[i][j] != INF && col[j] != INF) {
                reducedMatrix[i][j] -= col[j];
            }
        }
    }
    return 0;
}

int calculateCost(int reducedMatrix[N][N]) {
    int cost = 0;
    int row[N];
    rowReduction(reducedMatrix, row);
    int col[N];
    columnReduction(reducedMatrix, col);

    for (int i = 0; i < N; i++) {
        cost += (row[i] != INF) ? row[i] : 0;
        cost += (col[i] != INF) ? col[i] : 0;
    }
    return cost;
}

void TSPPathPrint(Node* list, int CostGraphMatrix[N][N]) {
    vector<bool> removedRows(N, false);
    vector<bool> removedCols(N, false);
    int stepMatrix = 0;
    int cost = 0;
    for (const auto& step : list->path) {
        int row = step.first;
        int col = step.second;
        stepMatrix++;
        cout << "Шаг: " << stepMatrix << endl;
        cout << "\t";
        for (int k = 0; k < N; k++) {
            if (!removedCols[k]) {
                cout << "\t" << k + 1;
            }
        }
        cout << endl;

        // Выводим текущую матрицу, исключая удалённые строки и столбцы
        for (int i = 0; i < N; i++) {
            if (!removedRows[i]) {
                cout << "\t" << i + 1 << "|";
                for (int j = 0; j < N; j++) {
                    if (!removedCols[j]) {
                        if  (CostGraphMatrix[i][j]== INF) {
                            // Удаляем строку и колонку
                            cout << "\t" << "INF "; // Можно заменить на нужный вам вывод или оставить пустым
                        }
                        else {
                            cout << "\t" << CostGraphMatrix[i][j] << " ";
                        }
                    }
                }
                cout << endl;
            }
            
        }
        cost += CostGraphMatrix[row][col];
        cout << "Сумма приводящих констант (rhk): " << cost << "\n";
        cout << "Оценка затрат: " << CostGraphMatrix[row][col] << "\n";
        cout << "Ребро маршрута (h, k): (" << row + 1 << ", " << col + 1 << ")\n";

        // Вычисляем штраф за неиспользование (phk)
        int penalty = 0;
        for (int i = 0; i < N; i++) {
            if (!removedRows[i]) {
                penalty += CostGraphMatrix[row][i];
            }
            if (!removedCols[i]) {
                penalty += CostGraphMatrix[i][col];
            }
        }

        cout << "Штраф за неиспользование (phk): " << penalty << "\n\n";

        // Обновляем статус удалённых строк и столбцов
        removedRows[row] = true; // Удаляем текущую строку
        removedCols[col] = true; // Удаляем текущий столбец
    }

    for (int i = 0; i < list->path.size(); i++) {
        cout << list->path[i].first + 1 << " -> " << list->path[i].second + 1 << "\n";
    }
}

struct Min_Heap {
    bool operator()(const Node* lhs, const Node* rhs) const {
        return lhs->cost > rhs->cost;
    }
};

int solve(int CostGraphMatrix[N][N]) {
    priority_queue<Node*, vector<Node*>, Min_Heap> pq;
    vector<pair<int, int>> v;

    Node* root = newNode(CostGraphMatrix, v, 0, -1, 0);
    root->cost = calculateCost(root->reducedMatrix);
    pq.push(root);

    int step = 1; // Шаги начинаются с 1

    while (!pq.empty()) {
        Node* min = pq.top();
        pq.pop();

        int i = min->vertex;

        if (min->level == N - 1) {
            min->path.push_back(make_pair(i, 0));
            TSPPathPrint(min, CostGraphMatrix);
            return min->cost;
        }

        for (int j = 0; j < N; j++) {
            if (min->reducedMatrix[i][j] != INF) {
                Node* child = newNode(min->reducedMatrix, min->path, min->level + 1, i, j);
                child->cost = min->cost + min->reducedMatrix[i][j] + calculateCost(child->reducedMatrix);
                theta.push_back(child->cost);
                pq.push(child);
            }
        }
    }
    return 0;
}

int main() {
    setlocale(LC_ALL, "russian");
    int CostGraphMatrix[N][N] = { { INF, 26, 18, 15, 3, 13 },
                                   { 24, INF, 19, 18, 7, 27 },
                                   { 5, 9, INF, 29, 24, 8 },
                                   { 13, 26, 20, INF, 26, 1 },
                                   { 0, 14, 15, 9, INF, 14 },
                                   { 9, 16, 21, 17, 16, INF } };

    cout << "Итого: " << solve(CostGraphMatrix);
    return 0;
}
