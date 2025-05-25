#include <iostream>
#include <vector>
#include <climits>
#include <cstring>
#include <algorithm>
using namespace std;

struct coordinates {
    int i;
    int j;
};

void sort(vector<int> & arr, vector<string>& helper) {
    bool sorted = false;
    while (!sorted) {
        sorted = true;
        for (int i = 0; i < arr.size() - 1; i++) {
            if (arr[i] > arr[i + 1]) {
                swap(arr[i], arr[i+1]);
                swap(helper[i], helper[i+1]);
                sorted = false;
            }
        }
    }
}

void neighboursAndDistance(const vector<int> &arr, vector<int>& indices, vector<int>& distances ) {
    for (int i = 0; i < arr.size(); i++) {
        if (arr[i] != 0) {
            indices.push_back(i);
            distances.push_back(arr[i]);
        }
    }
}

int findIndex(const vector<string>& helper,const string & target) {
    for (int i = 0; i < helper.size(); i++) {
        if (helper[i] == target) {
            return i;
        }
    }
    return -1;
}

class tile {
public:
    coordinates tileCoordinates;
    bool inPath;
    bool wall;
    tile() : tileCoordinates{0, 0}, inPath(false), wall(false) {}
    tile(int i, int j, bool w) {
        this->tileCoordinates = {i, j};
        this->tileCoordinates.i = i;
        this->tileCoordinates.j = j;
        this->wall = w;
        this->inPath = false;
    }
    void display() const {
        if (wall) {
            cout << "⬛";
        } else if (inPath) {
            cout << "🔵";
        }else {
            cout << "⬜";
        }
    }
};

class graph;

class maze {
public:
    vector<vector<tile>> mazeTiles;
    maze() {
        cout << "Enter maze rows: ";
        int rows;
        cin >> rows;
        cout << "Enter maze columns: ";
        int columns;
        cin >> columns;
        cout << "enter maze tiles row by row, 1 for walls, 0 for open tiles, make sure a space is between each tile: " << endl;
        for (int i = 0; i < rows; i++) {
            vector<tile> row(columns);
            cout << "enter row " << i + 1 << ": " << endl;
            bool invalid = false;
            for (int j = 0; j < columns; j++) {
                int in;
                cin >> in;
                if (in != 1 && in != 0) {
                    cout << "invalid tile type, reenter the row: " << endl;
                    row.clear();
                    i--;
                    invalid = true;
                    break;
                }else {
                    row[j] = tile(i, j, in == 1);
                }
            }
            if (!invalid) {
                mazeTiles.push_back(row);
            }
        }
    }
    void display() const {
        for (const auto& row: mazeTiles) {
            for (const auto& t: row) {
                t.display();
            }
            cout << endl;
        }
    }
    void solve();
    ~maze() {
        for ( auto row: mazeTiles) {
            row.clear();
        }
        mazeTiles.clear();
    }
};

class graph {
private:
    vector<string> nodeNames;
    vector<vector<int>> adjList;
    unsigned long long nodeCount;
public:
    // graph() {
    //     cout << "Enter number of nodes: ";
    //     int n;
    //     cin >> n;
    //     this->nodeCount = n;
    //     for (int i = 0; i < n; i++) {
    //         char currentNodeName;
    //         cout << "enter node #" << i + 1 << "'s name: ";
    //         cin >> currentNodeName;
    //         this->nodeNames.push_back(currentNodeName);
    //     }
    //     for (int i = 0; i < n; i++) {
    //         vector<int> tempVec;
    //         cout << "========================" << endl << "Node " << this->nodeNames[i] << " relationships:" << endl << "========================" << endl;
    //         for (int j = 0; j < n; j++) {
    //             if (i == j) {
    //                 tempVec.push_back(0);
    //                 continue;
    //             }
    //             cout << "enter node " << this->nodeNames[i] << "'s relationship with node " << this->nodeNames[j] << ", 0 for no relationship, >= 1 based on distance between them: ";
    //             int temp;
    //             cin >> temp;
    //             tempVec.push_back(temp);
    //         }
    //         adjList.push_back(tempVec);
    //     }
    // }
    ~graph() {
        nodeNames.clear();
        adjList.clear();
    }
    explicit graph(const maze& myMaze) {
        vector<tile> openTiles;
        for (const auto & row: myMaze.mazeTiles) {
            for (auto t: row) {
                if (!t.wall) {
                string name = "i" + to_string(t.tileCoordinates.i) + "j" + to_string(t.tileCoordinates.j);
                this->nodeNames.push_back(name);
                    openTiles.push_back(t);
                }
            }
        }
        this->nodeCount = openTiles.size();
        this->adjList = vector<vector<int>>(this->nodeCount, vector<int>(this->nodeCount, 0));
        for (int i = 0; i < this->nodeCount; i++) {
            for (int j = 0; j < this->nodeCount; j++) {
                adjList[i][j] = (openTiles[i].tileCoordinates.i == openTiles[j].tileCoordinates.i && (openTiles[i].tileCoordinates.j == openTiles[j].tileCoordinates.j + 1 || openTiles[i].tileCoordinates.j == openTiles[j].tileCoordinates.j - 1)) ? 1 : (openTiles[i].tileCoordinates.j == openTiles[j].tileCoordinates.j && (openTiles[i].tileCoordinates.i == openTiles[j].tileCoordinates.i + 1 || openTiles[i].tileCoordinates.i == openTiles[j].tileCoordinates.i - 1)) ? 1 : 0;
            }
        }
    }
    void display() const {
        cout << "Adjacency matrix:" << endl;
        cout << "  ";
        for (int i = 0; i < nodeCount; i++) {
            cout << this->nodeNames[i] << " ";
        }
        cout << endl;
        for (int i = 0; i < nodeCount; i++) {
            cout << this->nodeNames[i] << " ";
            for (int j = 0; j < nodeCount; j++) {
                cout << adjList[i][j] << " ";
            }
            cout << endl;
        }
    }
    vector <string> bfs(const string& start, const string & end) const {
        vector<vector<string>> queue = {{start}};
        int endIndex = findIndex(this->nodeNames, end);
        vector<bool> visited(this->nodeCount, false);
        while (!queue.empty()) {
            vector<string> currentPath = queue[0];
            queue.erase(queue.begin());
            int currentIndex = findIndex(this->nodeNames, currentPath[currentPath.size() - 1]);
            if (currentIndex == endIndex) {
                return currentPath;
            }
            for (int i = 0; i < this->nodeCount; i++) {
                if (adjList[currentIndex][i] != 0 && !visited[i]) {
                    vector<string> newPath = currentPath;
                    newPath.push_back(this->nodeNames[i]);
                    queue.push_back(newPath);
                    visited[i] = true;
                }
            }
        }
        return {"no path"};

    }
    [[nodiscard]] vector<string> dijkstra(const string& start, const string & end) const { // reference to my understanding: https://youtu.be/bZkzH5x0SKU?si=BYyPPRK8WC5liSkf
        int startIndex = findIndex(this->nodeNames, start);
        if (startIndex == -1) {
            cerr << "Invalid start node" << endl;
        }
        vector<int> distances(this->nodeCount, INT_MAX);
        distances[startIndex] = 0;
        vector<bool> visited(this->nodeCount, false);
        vector<string> previous(this->nodeCount, "no previous");
        for (int j = 0; j < this->nodeCount - 1; j++) {
            int shortestDistance = INT_MAX;
            int shortestIndex = -1;
            for (int i = 0; i < this->nodeCount; i++) { // Should also start from 0, not 1!
                if (!visited[i] && distances[i] < shortestDistance) {
                    shortestDistance = distances[i];
                    shortestIndex = i;
                }
            }
            if (shortestIndex == -1) break; // No more reachable nodes!
            vector<int> neighbourIndices;
            vector<int> distanceFromNeighbour;
            neighboursAndDistance(adjList[shortestIndex], neighbourIndices,distanceFromNeighbour);
            for (int i = 0; i < neighbourIndices.size(); i++) {
                if (distances[neighbourIndices[i]] > distanceFromNeighbour[i] + distances[shortestIndex]) {
                    distances[neighbourIndices[i]] = distanceFromNeighbour[i] + distances[shortestIndex];
                    previous[neighbourIndices[i]] = this->nodeNames[shortestIndex];
                }
            }
            visited[shortestIndex] = true;
        }
        int endIndex = findIndex(this->nodeNames, end);
        if (endIndex == -1) {
            cerr << "Invalid end node" << endl;
        }
        vector<string> path;
        int current = endIndex;
        while (current != -1 && previous[current] != "no previous") {
            path.push_back(this->nodeNames[current]);
            current = findIndex(this->nodeNames, previous[current]);
        }
        if (current != -1 && current == startIndex) {
            path.push_back(this->nodeNames[current]);
        }else {
            path.clear();
            path.emplace_back("no path");
        }
        reverse(path.begin(), path.end());
        return path;
    }
    [[nodiscard]] string getNodeName(const int& index) const {
        return this->nodeNames[index];
    }

};

coordinates extractCoordinates(const string& name) {
    coordinates c = {0, 0};
    int a = 1, b;
    char iC = name[a];

    while (iC != 'j') {
        c.i *= 10;
        c.i += iC - '0';
        a++;
        iC = name[a];
    }
    char jC = name[b = a + 1];
    while (jC != '\0') {
        c.j *= 10;
        c.j += jC - '0';
        jC = name[++b];
    }
    return c;
}

void maze::solve() {
    graph myGraph(*this);
    int si;
    int sj;
    int ei;
    int ej;
    cout << "Enter start row: ";
    cin >> si;
    cout << "Enter start column: ";
    cin >> sj;
    cout << "Enter end row: ";
    cin >> ei;
    cout << "Enter end column: ";
    cin >> ej;
    cout << "what kind of algorithm?: " << endl << "1. BFS" << endl << "2. Dijkstra" << endl << "enter your choice: ";
    int choice;
    cin >> choice;
    vector<string> path;
    string start = "i" + to_string(si - 1) + "j" + to_string(sj - 1), end = "i" + to_string(ei - 1) + "j" + to_string(ej - 1);
    switch (choice) {
        case 1:
            cout << "BFS path: " << endl;
            path = myGraph.bfs(start, end);
            break;
        case 2:
            cout << "Dijkstra path: " << endl;
            path = myGraph.dijkstra(start, end);
            break;
        default:
            cout << "Invalid choice" << endl;
            return;
    }
    if (path.size() == 1 && path[0] == "no path") {
        cout << "No path found" << endl;
        return;
    } else {
        for (const auto & t: path) {
            coordinates c = extractCoordinates(t);
            mazeTiles[c.i][c.j].inPath = true;
        }
    }
}

int main() {
    maze myMaze;
    myMaze.display();
    myMaze.solve();
    myMaze.display();
    return 0;
}
/*
----------Example 1 run:
Enter maze rows: 3
Enter maze columns: 3
enter maze tiles row by row, 1 for walls, 0 for open tiles, make sure a space is between each tile:
enter row 1:
1 1 2
invalid tile type, reenter the row:
enter row 1:
1 0 1
enter row 2:
1 0 0
enter row 3:
0 0 1
⬛⬜⬛
⬛⬜⬜
⬜⬜⬛
Enter start i: 0
Enter start j: 1
Enter end i: 1
Enter end j: 2
⬛🔵⬛
⬛🔵🔵
⬜⬜⬛

---------Example 2 run:
Enter maze rows: 5
Enter maze columns: 4
enter maze tiles row by row, 1 for walls, 0 for open tiles, make sure a space is between each tile:
enter row 1:
1 0 1 1
enter row 2:
1 0 0 0
enter row 3:
1 0 1 0
enter row 4:
1 0 1 0
enter row 5:
1 0 0 0
⬛⬜⬛⬛
⬛⬜⬜⬜
⬛⬜⬛⬜
⬛⬜⬛⬜
⬛⬜⬜⬜
Enter start i: 0
Enter start j: 1
Enter end i: 4
Enter end j: 2
⬛🔵⬛⬛
⬛🔵⬜⬜
⬛🔵⬛⬜
⬛🔵⬛⬜
⬛🔵🔵⬜

Process finished with exit code 0

---------Example 3 run:
Enter maze rows: 5
Enter maze columns: 5
enter maze tiles row by row, 1 for walls, 0 for open tiles, make sure a space is between each tile:
enter row 1:
1 0 0 1 1
enter row 2:
0 0 1 0 0
enter row 3:
1 0 0 1 0
enter row 4:
1 1 0 0 0
enter row 5:
0 0 0 1 1
⬛⬜⬜⬛⬛
⬜⬜⬛⬜⬜
⬛⬜⬜⬛⬜
⬛⬛⬜⬜⬜
⬜⬜⬜⬛⬛
Enter start row: 1
Enter start column: 2
Enter end row: 5
Enter end column: 1
what kind of algorithm?:
1. BFS
2. Dijkstra
enter your choice: 1
BFS path:
⬛🔵⬜⬛⬛
⬜🔵⬛⬜⬜
⬛🔵🔵⬛⬜
⬛⬛🔵⬜⬜
🔵🔵🔵⬛⬛


-------example 4 run:


Enter maze rows: 5
Enter maze columns: 5
enter maze tiles row by row, 1 for walls, 0 for open tiles, make sure a space is between each tile:
enter row 1:
1 0 0 1 1
enter row 2:
0 0 1 0 0
enter row 3:
1 0 0 1 0
enter row 4:
1 1 0 0 0
enter row 5:
0 0 0 1 1
⬛⬜⬜⬛⬛
⬜⬜⬛⬜⬜
⬛⬜⬜⬛⬜
⬛⬛⬜⬜⬜
⬜⬜⬜⬛⬛
Enter start row: 1
Enter start column: 2
Enter end row: 2
Enter end column: 4
what kind of algorithm?:
1. BFS
2. Dijkstra
enter your choice: 2
Dijkstra path:
⬛🔵⬜⬛⬛
⬜🔵⬛🔵🔵
⬛🔵🔵⬛🔵
⬛⬛🔵🔵🔵
⬜⬜⬜⬛⬛

Process finished with exit code 0

Process finished with exit code 0
*/