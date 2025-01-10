#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>

// алгоритм Тарждана для построения топологической сортировки ориентированного графа. Работает за О(n)

void get_top_sort_helper(const std::vector<std::vector<bool>>& adj_matrix, 
    std::vector<int>& stack, std::vector<bool>& used, unsigned vertice)
{
    used[vertice] = true;
    for (int i = 0; i < adj_matrix.size(); ++i)
    {
        if (adj_matrix[vertice][i] && !used[i])
        {
            get_top_sort_helper(adj_matrix, stack, used, i);
        }
    }
    stack.push_back(vertice);
}

std::vector<int> get_top_sort(const std::vector<std::vector<bool>>& adj_matrix)
{
    size_t sz = adj_matrix.size();
    std::vector<int> stack;
    stack.reserve(sz);
    std::vector<bool> used(sz, false);
    for (int i = 0; i < sz; ++i)
    {
        if (!used[i]) get_top_sort_helper(adj_matrix, stack, used, i);
    }
    std::reverse(stack.begin(), stack.end());
    return stack;
}

int main()
{
    std::vector<std::vector<bool>> adjacency_matrix = {
    //   0  1  2  3  4  5  6  7  8  9 куда идет ребро
    // по вертикали - откуда идет ребро
        {0, 1, 1, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
        {0, 1, 0, 0, 0, 0, 0, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 1, 0, 1},
        {0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    };
    std::vector<int> top_sort = get_top_sort(adjacency_matrix);
    for (auto x : top_sort) std::cout << x << ' ';
}
