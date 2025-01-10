#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>

// алгоритм Косараджу получения компонент сильносвязности орграфа. Работает за О(|V| + |E|), если граф
// представлен списком смежности, за O(|V|^2), если матрицей смежности

std::vector<std::vector<unsigned>> get_strongly_connected_components(const std::vector<std::vector<unsigned>>& adjacency_list)
{
    unsigned sz = adjacency_list.size();
    unsigned count = 0;
    std::vector<unsigned> t_out(sz, 0);
    std::vector<bool> is_grey(sz, false);
    std::vector<bool> is_black(sz, false);
    std::stack<unsigned> stack;
    // расстановка приоритетов вершинам
    for (int vertice = 0; vertice < sz; ++vertice)
    {
        if (!is_black[vertice])
        {
            stack.push(vertice);
            while (!stack.empty())
            {
                ++count;
                unsigned cur = stack.top();
                if (!is_grey[cur])
                {
                    is_grey[cur] = true;
                    for (int i = adjacency_list[cur].size() - 1; i >= 0; --i)
                    {
                        if (!is_grey[adjacency_list[cur][i]]) stack.push(adjacency_list[cur][i]);
                    }
                } else if (!is_black[cur]) {
                    is_black[cur] = true;
                    t_out[cur] = count;
                    stack.pop();
                } else {
                    stack.pop();
                }
            }            
        }
    }
    // for (auto x : t_out) std::cout << x << ' ';
    // упорядочивание вершин по убыванию приоритетов
    std::vector<std::pair<unsigned, unsigned>> vertices(sz);
    for (int i = 0; i < sz; ++i)
    {
        vertices[i] = std::make_pair(t_out[i], i);
    }
    std::sort(vertices.begin(), vertices.end(), [](auto a, auto b){ return a.first > b.first; });
    std::vector<std::vector<unsigned>> res;
    // std::cout << '\n';
    // for (auto x : vertices) std::cout << x.first << '_' << x.second << ' ';
    // std::cout << '\n';
    // постоение обратного графа
    std::vector<std::vector<unsigned>> adj_list_reversed(sz);
    for (int i = 0; i < sz; ++i)
    {
        for (int j = 0; j < adjacency_list[i].size(); ++j)
        {
            adj_list_reversed[adjacency_list[i][j]].push_back(i);
        }
    }
    // for (int i = 0; i < sz; ++i)
    // {
    //     std::cout << i << ": ";
    //     for (auto y : adj_list_reversed[i]) std::cout << y << ' ';
    //     std::cout << '\n';
    // }
    // прохождение поиском в глубину по всем непройденным вершинам в порядке убывания приоритета
    for (auto x : is_grey) x = false;
    for (auto x : is_black) x = false;
    for (auto it = vertices.begin(); it != vertices.end(); ++it)
    {
        if (!is_black[(*it).second])
        {
            // std::cout << (*it).second << ' ';
            res.push_back(std::vector<unsigned>());
            stack.push((*it).second);
            while (!stack.empty())
            {
                unsigned cur = stack.top();
                if (!is_grey[cur])
                {
                    is_grey[cur] = true;
                    res.back().push_back(cur);
                    for (int i = adj_list_reversed[cur].size() - 1; i >= 0; --i)
                    {
                        if (!is_grey[adj_list_reversed[cur][i]]) stack.push(adj_list_reversed[cur][i]);
                    }
                } else if (!is_black[cur]) {
                    is_black[cur] = true;
                    stack.pop();
                } else {
                    stack.pop();
                }
            }
        }
    }
    // std::cout << '\n';
    return res;
}

int main()
{
    // std::vector<std::vector<unsigned>> adj_list = {
    //     {1, 2},
    //     {0},
    //     {4},
    //     {2},
    //     {5},
    //     {2},
    // };
    // std::vector<std::vector<unsigned>> adj_list = {{}, {0, 2}, {1}};
    std::vector<std::vector<unsigned>> adj_list = 
    {
        {1},
        {2},
        {3, 5},
        {4},
        {1},
        {6},
        {7, 8, 10},
        {5},
        {7, 9},
        {10},
        {11, 6},
        {10, 12},
        {13},
        {12},
    };
    std::vector<std::vector<unsigned>> connected_components = get_strongly_connected_components(adj_list);
    for (auto x : connected_components)
    {
        for (auto y : x) std::cout << y << ' ';
        std::cout << '\n';
    } 
}