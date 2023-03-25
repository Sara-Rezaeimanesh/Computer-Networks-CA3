#include <iostream>
#include <vector>
#include <sstream>
#include <utility>
#include <math.h>
#include <limits.h>
#include <chrono>
#include <math.h>
#include <algorithm>
#include <string>

using namespace std;
using namespace std::chrono;

const char DELIM = '-';
const char *RANDOM_NUM = "-1";
const char *ARROW = "-->";
int nn = 0;
const int MAX_NODES = 2000;

struct Multiplier
{
    Multiplier(int i) : t(i) {}
    int t;
};

string operator*(string s, Multiplier m)
{
    string a = "";
    for (int i = 0; i < m.t; i++)
        a += s;
    return a;
}

class NetWork
{
public:
    void makeTopology(string topo);
    void showTopology();
    void lsrp(int src);
    void lsrpAll();
    void dvrp(int src);
    void dvrpAll();
    void removeLink(string input);
    void modifyLink(string input);

private:
    int getNextHop(int parent[], int j, int src);
    void printPath(int parent[], int j);
    void printPaths(int distance[], int parent[], int src);
    void printIteration(int dist[], int n, int parent[], int Iter);
    void printTable(int distance[], int parent[], int src);
    int findMinDistance(int dist[], bool visited[]);
    bool isPower(int x, long int y);
    vector<int> splitArgs(string args);

    int links[MAX_NODES][MAX_NODES];
    int n = 0;
};

bool NetWork::isPower(int x, long int y)
{
    long int pow = 1;
    while (pow < y)
        pow *= x;
    return (pow == y);
}

int NetWork::findMinDistance(int dist[], bool visited[])
{
    int min = INT_MAX, min_index;
    for (int i = 0; i < n; i++)
        if (!visited[i] && dist[i] <= min)
        {
            min = dist[i];
            min_index = i;
        }
    return min_index;
}

void NetWork::printPath(int parent[], int j)
{
    if (parent[j] == -1 || j == -1)
        return;

    printPath(parent, parent[j]);
    cout << j + 1 << ARROW;
}

int NetWork::getNextHop(int parent[], int j, int src)
{
    if (j <= 0)
        return j - 1;
    if (parent[j] == src)
        return j;
    if (parent[j] == -1)
        return j;
    return getNextHop(parent, parent[j], src);
}

void NetWork::printTable(int distance[], int parent[], int src)
{
    int col_space = 15;
    cout << "Dest           Next Hop       Dist           Shortest Path  \n";
    cout << "-----------------------------------------------------------------\n";
    
    for (int i = 0; i < n; i++)
    {
        int next_hop = getNextHop(parent, i, src);
        distance[i] = distance[i] == INT_MAX ? -1 : distance[i];
        cout << i+1;
        cout << string(col_space - to_string(i+1).size(), ' ');
        cout << next_hop + 1;
        cout << string(col_space - to_string(next_hop + 1).size(), ' ');
        cout << distance[i];
        cout << string(col_space - to_string(distance[i]).size(), ' ');
        cout << "[";
        cout << src + 1;
        if (src != i){
            cout << ARROW;
            printPath(parent, parent[i]); 
            cout << i + 1;
        }
        cout << "]\n";
        
    }
    cout << endl;
}

void NetWork::printPaths(int distance[], int parent[], int src)
{
    cout << "Path: [s] -> [d]     Min-Cost    Shortest Path\n";
    cout << "      ------------   ---------   -------------";
    for (int i = 0; i < n; i++)
    {
        if (i == src)
            continue;

        distance[i] = distance[i] == INT_MAX ? -1 : distance[i];
        int spaces = 12 - log10(distance[i] >= 0 ? distance[i] : 12);
        int spaces2 = 4 - ((isPower(10, i + 1)) ? log10(i + 1) : log10(i + 1) - 1);
        cout << "\n      [" << src + 1 << "] --> [" << i + 1 << "]" + " " * Multiplier(spaces2) << distance[i] << " " * Multiplier(spaces);

        cout << src + 1 << ARROW;

        printPath(parent, parent[i]);
        cout << i + 1;
    }
    cout << endl
         << endl;
}

void NetWork::printIteration(int dist[], int n, int parent[], int Iter)
{
    cout << "Iter " << Iter << ":\nDest   |";

    for (int i = 1; i <= n; i++)
        cout << " " << i << "|";

    cout << "\nCost   |";
    for (int i = 1; i <= n; i++)
    {
        if (dist[i - 1] == INT_MAX)
            cout << " -1|";
        else
            cout << " " << dist[i - 1] << "|";
    }
    cout << endl;
}

void NetWork::lsrp(int src)
{
    auto start = high_resolution_clock::now();
    auto mid = high_resolution_clock::now();

    int distance[MAX_NODES];
    bool visited[MAX_NODES] = {false};
    int parent[MAX_NODES] = {-1};

    for (int i = 0; i < n; i++)
        distance[i] = INT_MAX;
    distance[src] = 0;

    for (int iter = 0; iter < n - 1; iter++)
    {
        int u = findMinDistance(distance, visited);
        visited[u] = true;
        for (int v = 0; v < n; v++)
            if (!visited[v] && links[u][v] != -1 && distance[u] != INT_MAX && distance[u] + links[u][v] < distance[v])
            {
                parent[v] = u;
                distance[v] = distance[u] + links[u][v];
                mid = high_resolution_clock::now();
            }
        printIteration(distance, n, parent, iter + 1);
    }
    parent[src] = -1;
    printPaths(distance, parent, src);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    cout << "time elapsed for executing link state algorithm for node " << src + 1 << " = "
         << duration.count() << " microseconds\n\n";

    //uncomment to see the last time that our table was changed
    // auto duration2 = duration_cast<microseconds>(mid - start);
    // cout << "table while executing link state algorithm for node " << src + 1 << " was fixed after "
    //      << duration2.count() << " microseconds\n\n";
}

void NetWork::lsrpAll()
{
    auto start = high_resolution_clock::now();
    for (int i = 1; i <= n; i++)
        lsrp(i - 1);
    cout << endl;
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    cout << "time elapsed for executing link state algorithm for all nodes = "
         << duration.count() << " microseconds\n\n";
}

void NetWork::dvrp(int src)
{
    auto start = high_resolution_clock::now();
    auto mid = high_resolution_clock::now();

    int distance[MAX_NODES];
    int parent[MAX_NODES] = {-1};

    for (int i = 0; i < n; i++)
        distance[i] = INT_MAX;
    distance[src] = 0;
    for (int iter = 0; iter <= n - 1; iter++)
    {
        for (int i = 0; i < n; i++){
            for (int j = 0; j < n; j++){
                if (distance[i] != INT_MAX && links[i][j] != -1 && distance[i] + links[i][j] < distance[j]){
                    distance[j] = distance[i] + links[i][j];
                    parent[j] = i;
                    mid = high_resolution_clock::now();
                }
            }
        }
       
    }
    parent[src] = -1;
    printTable(distance, parent, src);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    cout << "time elapsed for executing distance vector algorithm for node " << src + 1 << " = "
         << duration.count() << " microseconds\n\n";

    //uncomment to see the last time that our table was changed
    // auto duration2 = duration_cast<microseconds>(mid - start);
    // cout << "table while executing distance vector algorithm for node " << src + 1 << " was fixed after "
    //      << duration2.count() << " microseconds\n\n";
}

void NetWork::dvrpAll()
{
    auto start = high_resolution_clock::now();
    for (int i = 1; i <= n; i++)
        dvrp(i - 1);
    cout << endl;
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    cout << "time elapsed for executing distance vector algorithm for all nodes = "
         << duration.count() << " microseconds\n\n";
}

vector<int> NetWork::splitArgs(string args)
{
    string u, v, w;
    stringstream ls(args);
    getline(ls, u, DELIM);
    getline(ls, v, DELIM);
    getline(ls, w, DELIM);
    w = w == "" ? RANDOM_NUM : w;
    return vector<int>{stoi(u) - 1, stoi(v) - 1, stoi(w)};
}

void NetWork::makeTopology(string topo)
{
    stringstream ts(topo);
    string linkInfo;

    for (int i = 0; i < MAX_NODES; i++)
        for (int j = 0; j < MAX_NODES; j++)
            links[i][j] = -1;

    for (int i = 0; i < MAX_NODES; i++)
        links[i][i] = 0;

    while (ts >> linkInfo)
    {
        vector<int> args = splitArgs(linkInfo);
        links[args[0]][args[1]] = links[args[1]][args[0]] = args[2];
        n = max(n, max(args[0] + 1, args[1] + 1));
    }
}

void NetWork::removeLink(string input)
{
    vector<int> args = splitArgs(input);
    links[args[0]][args[1]] = links[args[1]][args[0]] = -1;
}

void NetWork::modifyLink(string input)
{
    vector<int> args = splitArgs(input);
    if (args[0] == args[1])
    {
        cout << "can't modify the distance of a node from itself!";
        return;
    }
    links[args[0]][args[1]] = links[args[1]][args[0]] = args[2];
}

void NetWork::showTopology()
{
    cout << "u|v |  ";
    for (int i = 0; i < n; i++)
    {
        int spaces = i == 0 ? 2 : 3 - log10(i + 1);
        cout << " " * Multiplier(spaces) << i + 1 << " " * Multiplier(spaces);
    }
    cout << endl
         << "-------"
         << "-----" * Multiplier(n) << endl;

    for (int i = 0; i < n * 2; i++)
    {
        if (!(i % 2) && i != 0)
        {
            cout << "    | \n";
            continue;
        }
        else if (i == 0)
        {
            continue;
        }
        string spaces = (i + 1) / 2 >= 10 ? " " : "  ";
        cout << " " << (i + 1) / 2 << spaces + "|";
        for (int j = 0; j < n; j++)
        {
            int spaces = 5 - log10(links[i / 2][j] >= 0 ? max(9, links[i / 2][j]) : 12);
            cout << " " * Multiplier(spaces) << links[i / 2][j];
        }
        cout << endl;
    }
}

void handleCommands(NetWork *network)
{
    string command, args;
    while (cin >> command)
    {
        getline(cin, args);
        if (command == "topology")
            network->makeTopology(args);
        if (command == "show")
            network->showTopology();
        if (command == "lsrp")
            (args != "") ? network->lsrp(stoi(args) - 1) : network->lsrpAll();
        if(command == "dvrp")
            (args != "") ? network->dvrp(stoi(args) - 1) : network->dvrpAll();
        if (command == "remove")
            network->removeLink(args);
        if (command == "modify")
            network->modifyLink(args);
    }
}

int main()
{
    NetWork *network = new NetWork;
    handleCommands(network);
}