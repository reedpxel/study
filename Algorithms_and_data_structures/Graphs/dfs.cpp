#include <iostream>
#include <vector>
#include <stack>

// Поиск в глубину по графу. Если у графа больше одной компоненты связности, в void dfs(...) нужно вызываться от каждой компоненты
// Реализованы рекурсивные и нерекурсивные версии алгоритмов для матриц и список смежности
// Работает за O(|V| + |E|)

void dfs_rec(const std::vector<std::vector<bool>>& adjacency_matrix, std::vector<bool>& visited, size_t i)
{
    std::cout << i << ' ';
    visited[i] = true;
    for (int vertice = 0; vertice < adjacency_matrix.size(); ++vertice)
    {
        if (!visited[vertice] && adjacency_matrix[i][vertice]) dfs_rec(adjacency_matrix, visited, vertice);
    }
}

void dfs(const std::vector<std::vector<bool>>& adjacency_matrix, size_t index)
{
    std::vector<bool> visited(adjacency_matrix.size(), false);
    dfs_rec(adjacency_matrix, visited, index);
}

void dfs_on_adjacency_list_rec(const std::vector<std::vector<unsigned>>& adjacency_list, std::vector<bool>& visited, unsigned index)
{
    std::cout << index << ' ';
    visited[index] = true;
    for (int i = 0; i < adjacency_list[index].size(); ++i)
    {
        if (!visited[adjacency_list[index][i]])
        {
            dfs_on_adjacency_list_rec(adjacency_list, visited, adjacency_list[index][i]);
        }
    }
}

void dfs_on_adjacency_list(const std::vector<std::vector<unsigned>>& adjacency_list, unsigned index)
{
    std::vector<bool> visited(adjacency_list.size(), false);
    dfs_on_adjacency_list_rec(adjacency_list, visited, index);
}

void dfs_non_recursive(const std::vector<std::vector<bool>>& adjacency_matrix, unsigned index)
{
    size_t sz = adjacency_matrix.size();
    std::stack<int> stack;
    stack.push(index);
    std::vector<bool> is_grey(sz, false);
    std::vector<bool> is_black(sz, false);
    while (!stack.empty())
    {
        int cur = stack.top();
        if (!is_grey[cur])
        {
            std::cout << cur << ' ';
            is_grey[cur] = true;
            for (int i = sz - 1; i >= 0; --i)
            {
                if (adjacency_matrix[cur][i] && !is_grey[i]) stack.push(i);
            }
        } else if (!is_black[cur]) {
            is_black[cur] = true;
            stack.pop();
        } else {
            stack.pop();
        }
    }
}

void dfs_non_rec_adj_list(const std::vector<std::vector<unsigned>>& adj_list, unsigned index)
{
    size_t sz = adj_list.size();
    std::stack<int> stack;
    stack.push(index);
    std::vector<bool> is_grey(sz, false);
    std::vector<bool> is_black(sz, false);
    while (!stack.empty())
    {
        unsigned cur = stack.top();
        if (!is_grey[cur])
        {
            std::cout << cur << ' ';
            is_grey[cur] = true;
            for (int i = adj_list[cur].size() - 1; i >= 0; --i)
            {
                if (!is_grey[adj_list[cur][i]]) stack.push(adj_list[cur][i]);
            }
        } else if (!is_black[cur]) {
            is_black[cur] = true;
            stack.pop();
        } else {
            stack.pop();
        }
    }
}

int main()
{
    std::vector<std::vector<bool>> m = {{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                        {1, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0},
                                        {0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1},
                                        {0, 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0},
                                        {0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0},
                                        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0},
                                        {0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0},
                                        {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
                                        {0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0},
                                        {0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0},
                                        {0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0},
                                        {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
    dfs(m, 5);
    std::cout << '\n';
    dfs_non_recursive(m, 5);
    std::cout << '\n';
    std::vector<std::vector<unsigned>> adj_list = {{1},
                                                   {0, 3, 4, 5, 7},
                                                   {3, 7},
                                                   {1, 2, 4, 6},
                                                   {1, 3, 5, 6},
                                                   {1, 4, 6},
                                                   {3, 4, 5, 7},
                                                   {1, 2, 6, 8, 9},
                                                   {4, 7},
                                                   {7}};
    dfs_on_adjacency_list(adj_list, 0);
    std::cout << '\n';
    dfs_non_rec_adj_list(adj_list, 0);
}