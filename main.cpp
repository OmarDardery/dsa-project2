#include <iostream>
#include <vector>
using namespace std;

void sort(vector<int> & arr, vector<char>& helper) {
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

void neighboursAndDistance(const vector<int> &arr,const vector<char>& helper, vector<char>& neighbours, vector<int>& distances ) {
    for (int i = 0; i < arr.size(); i++) {
        if (arr[i] != 0) {
            neighbours.push_back(helper[i]);
            distances.push_back(arr[i]);
        }
    }
}

int findIndex(const vector<char>& helper,const char & target) {
    for (int i = 0; i < helper.size(); i++) {
        if (helper[i] == target) {
            return i;
        }
    }
    return -1;
}

class graph {
private:
    vector<char> nodeNames;
    vector<vector<int>> adjList;
    unsigned long long nodeCount;
public:
    graph() {
        cout << "Enter number of nodes: ";
        int n;
        cin >> n;
        this->nodeCount = n;
        for (int i = 0; i < n; i++) {
            char currentNodeName;
            cout << "enter node #" << i + 1 << "'s name: ";
            cin >> currentNodeName;
            this->nodeNames.push_back(currentNodeName);
        }
        for (int i = 0; i < n; i++) {
            vector<int> tempVec;
            cout << "========================" << endl << "Node " << this->nodeNames[i] << " relationships:" << endl << "========================" << endl;
            for (int j = 0; j < n; j++) {
                if (i == j) {
                    tempVec.push_back(0);
                    continue;
                }
                cout << "enter node " << this->nodeNames[i] << "'s relationship with node " << this->nodeNames[j] << ", 0 for no relationship, >= 1 based on distance between them: ";
                int temp;
                cin >> temp;
                tempVec.push_back(temp);
            }
            adjList.push_back(tempVec);
        }
    }
    ~graph() {
        nodeNames.clear();
        adjList.clear();
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
    [[nodiscard]] vector<int> dijkstra(const char& start, const char & end) const { // reference to my understanding: https://youtu.be/bZkzH5x0SKU?si=BYyPPRK8WC5liSkf
        int startIndex = -1;
        for (int i = 0; i < this->nodeCount; i++) {
            if (this->nodeNames[i] == start) {
                startIndex = i;
            }
        }
        vector<int> distances(this->nodeCount, INT_MAX);
        distances[startIndex] = 0;
        vector<bool> visited(this->nodeCount, false);
        visited[startIndex] = true;
        vector<char> previous(this->nodeCount, ' ');
        vector<char> queue = this->nodeNames;
        vector<int> queueDistances = distances;
        for (int j = 0; j < this->nodeCount - 1; j++) {
            sort(queueDistances, queue);
            vector<char> neighbours;
            vector<int> neighbourDistances;
            int currentIndex = findIndex(this->nodeNames, queue[0]);
            neighboursAndDistance(adjList[currentIndex], this->nodeNames, neighbours, neighbourDistances);
            for (int i = 0; i < neighbours.size(); i++) {
                int neighbourIndex = findIndex(this->nodeNames, neighbours[i]);
                if (distances[neighbourIndex] > neighbourDistances[i] + distances[currentIndex]) {
                    distances[neighbourIndex] = neighbourDistances[i] + distances[currentIndex];
                    previous[neighbourIndex] = this->nodeNames[currentIndex];
                }
            }
            visited[currentIndex] = true;
            queue.erase(queue.begin());
            queueDistances.erase(queueDistances.begin());

            sort(queueDistances, queue);
        }
        return distances;
    }
    [[nodiscard]] char getNodeName(const int& index) const {
        return this->nodeNames[index];
    }

};

int main() {
    graph g;
    g.display();
    cout << "Enter start node: ";
    char start;
    cin >> start;
    cout << "Enter end node: ";
    char end;
    cin >> end;
    vector<int> distances = g.dijkstra(start, end);
    for (int i = 0; i < distances.size(); i++) {
        cout << g.getNodeName(i) << ": " << distances[i] << endl;
    }
    return 0;
}