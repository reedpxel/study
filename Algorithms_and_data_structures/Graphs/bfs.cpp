#include <iostream>
#include <vector>
#include <queue>

// Поиск в ширину. Рекурсивной реализации алгоритма не бывает. Работает за О(|V|^2) для матрицы сопряженности,
// за О(|V| + |E|) для списка сопряженности

void bfs(const std::vector<std::vector<bool>>& adjacency_matrix, unsigned beginning_index)
{
    size_t sz = adjacency_matrix.size();
    std::queue<unsigned> bfs_queue;
    std::vector<bool> used(sz, false);
    bfs_queue.push(beginning_index);
    while (!bfs_queue.empty())
    {
        unsigned cur = bfs_queue.front();
        std::cout << cur << ' ';
        bfs_queue.pop();
        used[cur] = true;
        for (int i = 0; i < sz; ++i)
        {
            if (adjacency_matrix[cur][i] && !used[i])
            {
                bfs_queue.push(i);
                used[i] = true;
            }
        }
    }
}

void bfs(const std::vector<std::vector<unsigned>>& adjacency_list, unsigned beginning_index)
{
    size_t sz = adjacency_list.size();
    std::queue<unsigned> bfs_queue;
    std::vector<bool> used(sz, false);
    bfs_queue.push(beginning_index);
    while (!bfs_queue.empty())
    {
        unsigned cur = bfs_queue.front();
        std::cout << cur << ' ';
        bfs_queue.pop();
        used[cur] = true;
        for (unsigned neighbour : adjacency_list[cur])
        {
            if (!used[neighbour])
            {
                bfs_queue.push(neighbour);
                used[neighbour] = true;
            }
        }
    }
}

int main()
{
    std::vector<std::vector<bool>> adjacency_matrix = 
    {       //0  1  2  3  4  5  6  7  8  9
    /*0*/    {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
    /*1*/    {0, 0, 1, 1, 0, 0, 0, 0, 0, 1},
    /*2*/    {0, 1, 0, 1, 1, 0, 0, 0, 1, 0},
    /*3*/    {0, 1, 1, 0, 0, 1, 0, 0, 1, 1},
    /*4*/    {0, 0, 1, 0, 0, 0, 0, 1, 0, 0},
    /*5*/    {1, 0, 0, 1, 0, 0, 1, 0, 0, 0},
    /*6*/    {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
    /*7*/    {0, 0, 0, 0, 1, 0, 0, 0, 1, 0},
    /*8*/    {0, 0, 1, 1, 0, 0, 0, 1, 0, 1},
    /*9*/    {0, 1, 0, 1, 0, 0, 0, 0, 1, 0}
    };
    std::vector<std::vector<unsigned>> adjacency_list = 
    {
    /*0*/ {5},
    /*1*/ {2, 3, 9},
    /*2*/ {1, 3, 4, 8},
    /*3*/ {1, 2, 5, 8, 9},
    /*4*/ {2, 7},
    /*5*/ {0, 3, 6},
    /*6*/ {5},
    /*7*/ {4, 8},
    /*8*/ {2, 3, 7, 9},
    /*9*/ {1, 3, 8}
    };  
    bfs(adjacency_matrix, 0);
    std::cout << '\n';
    bfs(adjacency_list, 0);
}