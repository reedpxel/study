#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <algorithm>

using std::vector;

class Network
{
public:
    vector<vector<int>> adj_matrix_w_capacities;
    int source;
    int sink;

    vector<int> get_path_w_dfs(
        vector<vector<int>>& remaining_network,
        vector<vector<int>>& max_flow);
    vector<int> get_path_w_bfs(
        vector<vector<int>>& remaining_network,
        vector<vector<int>>& max_flow);
    vector<int> get_path_in_layered_network(
        vector<vector<int>>& network);
    
public:
    Network(const vector<vector<int>>& adjacency_matrix, 
        int source,
        int sink);
    int size();
    vector<vector<int>> get_max_flow_Ford_Fulkerson(int& value_of_flow);
    vector<vector<int>> get_max_flow_Endmonds_Karp(int& value_of_flow);
    vector<vector<int>> get_max_flow_Dinic(int& value_of_flow);
};

vector<int> Network::get_path_w_dfs(
    vector<vector<int>>& remaining_network,
    vector<vector<int>>& max_flow)
{
    std::stack<int> stack;
    vector<int> ancestors(size(), -1);
    stack.push(source);
    vector<bool> is_grey(size(), false);
    vector<bool> is_black(size(), false);
    while (!stack.empty())
    {
        int cur = stack.top();
        if (cur == sink)
        {
            std::deque<int> path_deque = {sink};
            int anc = ancestors[sink];
            while (anc != -1)
            {
                path_deque.push_front(anc);
                anc = ancestors[anc];
            }
            for (int i = 0; i < path_deque.size() - 1; ++i)
            {
                int u = path_deque[i];
                int v = path_deque[i + 1];
                --remaining_network[u][v];
                ++remaining_network[v][u];
                ++max_flow[u][v];
                --max_flow[v][u];
            }
            return vector(path_deque.begin(), path_deque.end());
        }
        if (!is_grey[cur])
        {
            is_grey[cur] = true;
            for (int i = size() - 1; i >= 0; --i)
            {
                if (!remaining_network[cur][i] || is_grey[i]) continue;
                stack.push(i);
                ancestors[i] = cur;
            }
        } else if (!is_black[cur]) {
            is_black[cur] = true;
            stack.pop();
        } else {
            stack.pop();
        }
    }
    return {};
}

vector<int> Network::get_path_w_bfs(
    vector<vector<int>>& remaining_network,
    vector<vector<int>>& max_flow)
{
    std::queue<int> bfs_queue;
    vector<bool> used(size(), false);
    vector<int> ancestors(size(), -1);
    bfs_queue.push(source);
    while (!bfs_queue.empty())
    {
        unsigned cur = bfs_queue.front();
        if (cur == sink)
        {
            std::deque<int> path_deque = {sink};
            int anc = ancestors[sink];
            while (anc != -1)
            {
                path_deque.push_front(anc);
                anc = ancestors[anc];
            }
            for (int i = 0; i < path_deque.size() - 1; ++i)
            {
                int u = path_deque[i];
                int v = path_deque[i + 1];
                --remaining_network[u][v];
                ++remaining_network[v][u];
                ++max_flow[u][v];
                --max_flow[v][u];
            }
            return vector(path_deque.begin(), path_deque.end());
        }
        bfs_queue.pop();
        used[cur] = true;
        for (int i = 0; i < size(); ++i)
        {
            if (!remaining_network[cur][i] || used[i]) continue;
            bfs_queue.push(i);
            used[i] = true;
            ancestors[i] = cur;
        }
    }
    return {};
}

Network::Network(const vector<vector<int>>& matrix_with_capacities, 
    int source,
    int sink) :
    adj_matrix_w_capacities(matrix_with_capacities),
    source(source),
    sink(sink) {}

int Network::size() { return adj_matrix_w_capacities.size(); }

// работает за O(|V||E|^2)
vector<vector<int>> Network::get_max_flow_Ford_Fulkerson(int& value_of_flow)
{
    vector<vector<int>> max_flow(size(), vector<int>(size()));
    vector<vector<int>> remaining_network = adj_matrix_w_capacities;
    vector<int> path = {-1};
    while (path.size())
    {
        path = get_path_w_dfs(remaining_network, max_flow);
    }
    value_of_flow = 0;
    for (int i = 0; i < size(); ++i)
    {
        value_of_flow += max_flow[source][i];
    }
    return max_flow;
}

// работает за O(|V||E|^2)
vector<vector<int>> Network::get_max_flow_Endmonds_Karp(int& value_of_flow)
{
    vector<vector<int>> max_flow(size(), vector<int>(size()));
    vector<vector<int>> remaining_network = adj_matrix_w_capacities;
    vector<int> path = {-1};
    while (path.size())
    {
        path = get_path_w_bfs(remaining_network, max_flow);
    }
    value_of_flow = 0;
    for (int i = 0; i < size(); ++i)
    {
        value_of_flow += max_flow[source][i];
    }
    return max_flow;
}

// работает за O(|V|^2|E|)
/*
    Алгоритм:
    0) max_flow тождественно = 0
    1) строим слоистую сеть
    2) находим любой путь в слоистой сети, попутно находим ребро минимальной
емкости
    3) вычитаем емкость мин. ребра из всех ребер самого графа (не слоистой сети)
на пути, соотв. ребра в max_flow увел. на эту величину
    4) повторяем шаги 1 - 4
*/
vector<vector<int>> Network::get_max_flow_Dinic(int& value_of_flow)
{
    vector<vector<int>> max_flow(size(), vector<int>(size()));
    vector<vector<int>> adj_matrix = adj_matrix_w_capacities;
    vector<vector<int>> layered_network;
    for (int cycle = 0; cycle < size(); ++cycle)
    {
        layered_network = adj_matrix;
        vector<int> path = get_path_in_layered_network(layered_network);
        if (path.empty()) break;
        int min_edge_length = INT_MAX;
        for (int i = 0; i < path.size() - 1; ++i)
        {
            if (min_edge_length > layered_network[path[i]][path[i + 1]])
            {
                min_edge_length = layered_network[path[i]][path[i + 1]];
            }
        }
        for (int i = 0; i < path.size() - 1; ++i)
        {
            adj_matrix[path[i]][path[i + 1]] -= min_edge_length;
            max_flow[path[i]][path[i + 1]] += min_edge_length;
        }
    }
    for (int i = 0; i < size(); ++i)
    {
        value_of_flow += max_flow[source][i];
    }
    return max_flow;
}

vector<int> Network::get_path_in_layered_network(
    vector<vector<int>>& network)
{
    std::queue<int> bfs_queue;
    std::vector<bool> used(size(), false);
    std::vector<int> depth(size());
    std::vector<int> ancestors(size(), -1);
    bfs_queue.push(source);
    while (!bfs_queue.empty())
    {
        int cur = bfs_queue.front();
        bfs_queue.pop();
        used[cur] = true;
        for (int i = 0; i < size(); ++i)
        {
            if (network[cur][i] && !used[i])
            {
                bfs_queue.push(i);
                used[i] = true;
                depth[i] = depth[cur] + 1;
                ancestors[i] = cur;
            }
        }
    }
    if (ancestors[sink] == -1) return {};
    std::deque<int> path = {sink};
    int anc = ancestors[sink];
    while (anc != -1)
    {
        path.push_front(anc);
        anc = ancestors[anc];
    }
    for (int i = 0; i < size(); ++i)
    {
        for (int j = 0; j < size(); ++j)
        {
            if (network[i][j] && depth[i] - depth[j] != -1)
            {
                network[i][j] = 0;
            }
        }
    }
    return vector(path.begin(), path.end());
}

int main()
{
    vector<vector<int>> m = 
    {
        {0, 3, 2, 0},
        {0, 0, 1, 2},
        {0, 0, 0, 3},
        {0, 0, 0, 0}
    };
    vector<vector<int>> m2 = 
    {    //0  1  2  3  4  5  6  7  8  9  10   
    /*0*/ {0, 5, 5, 5, 0, 0, 0, 0, 0, 0, 0},
    /*1*/ {0, 0, 3, 0, 0, 2, 0, 0, 0, 0, 0},
    /*2*/ {0, 0, 0, 4, 0, 7, 3, 0, 0, 0, 0},
    /*3*/ {0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0},
    /*4*/ {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    /*5*/ {0, 0, 0, 0, 9, 0, 0, 0, 5, 0, 0},
    /*6*/ {0, 0, 0, 0, 0, 0, 0, 3, 0, 8, 0},
    /*7*/ {0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 8},
    /*8*/ {0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0},
    /*9*/ {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7},
    /*10*/{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };
    Network n(m, 0, 3);
    Network n2(m2, 0, 10);
    int value_of_flow = 0;
    auto v = n2.get_max_flow_Dinic(value_of_flow);
    std::cout << value_of_flow << '\n';
    for (auto x : v)
    {
        for (auto y : x) std::cout << y << ' ';
        std::cout << '\n';
    }
}