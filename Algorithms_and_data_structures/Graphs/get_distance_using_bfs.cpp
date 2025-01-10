#include <iostream>
#include <vector>
#include <queue>
#include <deque>

std::vector<unsigned> get_distances(
    const std::vector<std::vector<bool>>& adjacency_matrix,
    unsigned vertice)
{
    size_t sz = adjacency_matrix.size();
    std::queue<unsigned> bfs_queue;
    std::vector<bool> used(sz, false);
    std::vector<unsigned> res(sz, 0);
    unsigned count = 0;
    bfs_queue.push(vertice);
    while (!bfs_queue.empty())
    {
        unsigned cur = bfs_queue.front();
        bfs_queue.pop();
        used[cur] = true;
        for (int i = 0; i < sz; ++i)
        {
            if (adjacency_matrix[cur][i] && !used[i])
            {
                bfs_queue.push(i);
                used[i] = true;
                res[i] = res[cur] + 1;
            }
        }
    }
    return res;
}

std::vector<unsigned> _0_1_bfs(
    const std::vector<std::vector<unsigned>>& adjacency_list, 
    const std::vector<std::vector<bool>>& edges_length, unsigned vertice)
{
    size_t sz = adjacency_list.size();
    std::deque<unsigned> bfs_deque;
    std::vector<bool> used(sz, false);
    std::vector<unsigned> res(sz, 0);
    unsigned count = 0;
    bfs_deque.push_back(vertice);
    while (!bfs_deque.empty())
    {
        unsigned cur = bfs_deque.front();
        bfs_deque.pop_front();
        used[cur] = true;
        for (int i = 0; i < adjacency_list[cur].size(); ++i)
        {
            if (!used[adjacency_list[cur][i]])
            {
                if (edges_length[cur][i])
                {
                    bfs_deque.push_back(adjacency_list[cur][i]);
                    res[adjacency_list[cur][i]] = res[cur] + 1;
                } else {
                    bfs_deque.push_front(adjacency_list[cur][i]);
                    res[adjacency_list[cur][i]] = res[cur];
                }
                used[adjacency_list[cur][i]] = true;
            }
        }
    }
    return res;
}

// 1-k BFS работает за O(N * K + M)

std::vector<unsigned> _1_k_bfs(
    const std::vector<std::vector<unsigned>>& adjacency_list, 
    const std::vector<std::vector<unsigned>>& edges_length, unsigned vertice)
{
    size_t sz = adjacency_list.size();
    unsigned k = 1;
    for (auto v : edges_length)
    {
        for (auto x : v)
        {
            if (x > k) k = x; 
        }
    }
    std::vector<std::queue<unsigned>> deque_array((sz - 1) * k, std::queue<unsigned>());
    deque_array[0].push(vertice);
    std::vector<unsigned> res(sz, 0);
    std::vector<bool> used(sz, false);
    unsigned pos = 0, vertices_in_queues = 1;
    while (vertices_in_queues)
    {
        while (deque_array[pos].empty()) ++pos;
        unsigned cur = deque_array[pos].front();
        deque_array[pos].pop();
        --vertices_in_queues;
        if (used[cur]) continue;
        used[cur] = true;
        res[cur] = pos;
        for (int i = 0; i < adjacency_list[cur].size(); ++i)
        {
            if (used[adjacency_list[cur][i]]) continue;
            deque_array[pos + edges_length[cur][i]].push(adjacency_list[cur][i]);
            ++vertices_in_queues;
        }
    }
    return res;
}

int main()
{
    // std::vector<std::vector<bool>> adjacency_matrix = 
    // {       //0  1  2  3  4  5  6  7  8  9
    // /*0*/    {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
    // /*1*/    {0, 0, 1, 1, 0, 0, 0, 0, 0, 1},
    // /*2*/    {0, 1, 0, 1, 1, 0, 0, 0, 1, 0},
    // /*3*/    {0, 1, 1, 0, 0, 1, 0, 0, 1, 1},
    // /*4*/    {0, 0, 1, 0, 0, 0, 0, 1, 0, 0},
    // /*5*/    {1, 0, 0, 1, 0, 0, 1, 0, 0, 0},
    // /*6*/    {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
    // /*7*/    {0, 0, 0, 0, 1, 0, 0, 0, 1, 0},
    // /*8*/    {0, 0, 1, 1, 0, 0, 0, 1, 0, 1},
    // /*9*/    {0, 1, 0, 1, 0, 0, 0, 0, 1, 0}
    // };
    // std::vector<std::vector<unsigned>> adjacency_list = 
    // {
    // /*0*/ {5},
    // /*1*/ {2, 3, 9},
    // /*2*/ {1, 3, 4, 8},
    // /*3*/ {1, 2, 5, 8, 9},
    // /*4*/ {2, 7},
    // /*5*/ {0, 3, 6},
    // /*6*/ {5},
    // /*7*/ {4, 8},
    // /*8*/ {2, 3, 7, 9},
    // /*9*/ {1, 3, 8}
    // };
    // std::vector<std::vector<bool>> edges_length = 
    // {
    // /*0*/ {1},
    // /*1*/ {0, 1, 0},
    // /*2*/ {0, 1, 1, 1},
    // /*3*/ {1, 1, 0, 0, 1},
    // /*4*/ {1, 1},
    // /*5*/ {1, 0, 1},
    // /*6*/ {1},
    // /*7*/ {1, 1},
    // /*8*/ {1, 0, 1, 1},
    // /*9*/ {0, 1, 1}
    // };
    // std::vector<std::vector<unsigned>> edges_length = 
    // {
    // /*0*/ {3},
    // /*1*/ {1, 4, 2},
    // /*2*/ {1, 4, 3, 3},
    // /*3*/ {4, 4, 1, 1, 3},
    // /*4*/ {3, 5},
    // /*5*/ {3, 1, 2},
    // /*6*/ {2},
    // /*7*/ {5, 1},
    // /*8*/ {3, 1, 1, 2},
    // /*9*/ {2, 3, 2}
    // };
    // std::vector<unsigned> distances = get_distances(adjacency_matrix, 0);
    // for (unsigned i : distances) std::cout << i << ' ';
    // std::vector<unsigned> distances = _0_1_bfs(adjacency_list, edges_length, 0);
    // for (auto x : distances) std::cout << x << ' ';
    std::vector<std::vector<unsigned>> adjacency_list = 
    {
        {1, 2},
        {0, 3},
        {0, 3},
        {1, 2}
    };
    std::vector<std::vector<unsigned>> edges_length = 
    {
        {5, 1},
        {5, 1},
        {1, 1},
        {1, 1}
    };
    std::vector<unsigned> distances = _1_k_bfs(adjacency_list, edges_length, 2);
    for (auto x : distances) std::cout << x << ' '; 
}