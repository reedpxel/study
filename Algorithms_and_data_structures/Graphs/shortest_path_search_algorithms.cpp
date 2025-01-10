#include <iostream>
#include <vector>
#include <set>
#include <limits>
#include <queue>
#include <deque>
#include <functional>
#include <fstream>

// Проверка неорграфа на отсутствие ошибок:

    // for (int i = 0; i < adjacency_list_4.size(); ++i)
    // {
    //     for (int j = 0; j < adjacency_list_4[i].size(); ++j)
    //     {
    //         int _i = 0;
    //         unsigned lst = adjacency_list_4[i][j];
    //         while (_i != adjacency_list_4[lst].size() && 
    //             lst != adjacency_list_4[lst][_i])
    //         {
    //             ++_i;
    //         }
    //         if (_i != adjacency_list_4[lst].size()) std::cout << i << ' ' << j << '\n';
    //     }
    // }

class WeightedGraph
{
    unsigned vertices;
    std::vector<std::vector<unsigned>> adjacency_list;
    std::vector<std::vector<double>> edges_length;
public:
    unsigned size();
    WeightedGraph(const std::vector<std::vector<unsigned>>& adjacency_list,
        const std::vector<std::vector<double>>& edges_length);

    std::vector<double> get_distances_Dijkstras_algorithm(unsigned vertice);
    std::vector<double> get_distances_Bellman_Ford_algorithm(unsigned vertice);
    std::vector<std::vector<double>> get_APSP();
    std::pair<double, std::vector<unsigned>> get_path(unsigned v1, unsigned v2);
    std::vector<double> Dijkstra_on_set(unsigned v);
};

class Graph
{
    unsigned vertices;
    std::vector<std::vector<unsigned>> adjacency_list;
    std::vector<unsigned> get_ans(
        unsigned v1, 
        unsigned v2,
        unsigned common_vertice,
        const std::vector<int>& ancestors1,
        const std::vector<int>& ancestors2);
public:
    unsigned size();
    Graph(const std::vector<std::vector<unsigned>>& adjacency_list);
    std::vector<unsigned> get_path_one_side_bfs(unsigned v1, unsigned v2);
    std::vector<unsigned> get_path_two_side_bfs(unsigned v1, unsigned v2);
};

Graph::Graph(const std::vector<std::vector<unsigned>>& adjacency_list) : 
    vertices(adjacency_list.size()),
    adjacency_list(adjacency_list) {}

unsigned Graph::size() { return vertices; }

unsigned WeightedGraph::size() { return vertices; }

// возвращает путь по вершинам от v1 до v2. При желании можно использовать 1-k bfs для
// взвешенного графа, но есть лучшие алгоритмы

std::vector<unsigned> Graph::get_path_one_side_bfs(unsigned v1, unsigned v2)
{
    if (v1 == v2) return {v1};
    std::vector<int> ancestors(vertices, -1);
    ancestors[v2] = 0;
    std::queue<unsigned> bfs_queue;
    bfs_queue.push(v2);
    while (!bfs_queue.empty())
    {
        unsigned cur = bfs_queue.front();
        bfs_queue.pop();
        for (unsigned neighbour : adjacency_list[cur])
        {
            if (ancestors[neighbour] >= 0) continue;
            ancestors[neighbour] = cur;
            if (neighbour == v1)
            {
                std::vector<unsigned> res = {v1};
                unsigned nbr = ancestors[neighbour];
                while (nbr != v2)
                {
                    res.push_back(nbr);
                    nbr = ancestors[nbr];
                }
                res.push_back(v2);
                return res;
            }
            bfs_queue.push(neighbour);
        }
    }
    return {vertices};
}

// то же, что односторонный bfs, но запускает одновременно два bfs-a, за счет 
// чего уменьшается кол-во вершин, уходящих в сторону

std::vector<unsigned> Graph::get_path_two_side_bfs(unsigned v1, unsigned v2)
{
    if (v1 == v2) return {v1};
    std::vector<int> ancestors1(vertices, -1);
    std::vector<int> ancestors2(vertices, -1);
    ancestors1[v1] = 0;
    ancestors2[v2] = 0;
    std::queue<unsigned> bfs1_queue, bfs2_queue;
    bfs1_queue.push(v1);
    bfs2_queue.push(v2);
    while (!bfs1_queue.empty() && !bfs2_queue.empty())
    {
        unsigned cur1 = bfs1_queue.front();
        unsigned cur2 = bfs2_queue.front();
        if (cur1 == cur2) return get_ans(v1, v2, cur1, ancestors1, ancestors2);
        bfs1_queue.pop();
        bfs2_queue.pop();
        for (unsigned nbr : adjacency_list[cur1])
        {
            if (ancestors1[nbr] >= 0) continue;
            ancestors1[nbr] = cur1;
            if (ancestors2[nbr] >= 0 || nbr == v2)
            {
                return get_ans(v1, v2, nbr, ancestors1, ancestors2);
            }
            bfs1_queue.push(nbr);
        }
        for (unsigned nbr : adjacency_list[cur2])
        {
            if (ancestors2[nbr] >= 0) continue;
            ancestors2[nbr] = cur2;
            if (ancestors1[nbr] >= 0 || nbr == v1)
            {
                return get_ans(v1, v2, nbr, ancestors1, ancestors2);
            }
            bfs2_queue.push(nbr);
        }
    }
    return {vertices};
}

std::vector<unsigned> Graph::get_ans(
    unsigned v1, 
    unsigned v2,
    unsigned common_vertice,
    const std::vector<int>& ancestors1,
    const std::vector<int>& ancestors2)
{
    if (common_vertice == v1 || common_vertice == v2) return {v1, v2};
    std::deque<unsigned> deque_tmp;
    deque_tmp.push_back(common_vertice);
    unsigned nbr = ancestors1[common_vertice];
    while (nbr != v1)
    {
        deque_tmp.push_front(nbr);
        nbr = ancestors1[nbr];
    }
    deque_tmp.push_front(v1);
    nbr = ancestors2[common_vertice];
    while (nbr != v2)
    {
        deque_tmp.push_back(nbr);
        nbr = ancestors2[nbr];
    }
    deque_tmp.push_back(v2);
    return std::vector<unsigned>(deque_tmp.begin(), deque_tmp.end());
}

WeightedGraph::WeightedGraph(const std::vector<std::vector<unsigned>>& adjacency_list,
    const std::vector<std::vector<double>>& edges_length) :
    vertices(adjacency_list.size()), 
    adjacency_list(adjacency_list),
    edges_length(edges_length) {}

// Алгоритм Дейкстры (именно эта реализация) работает за О(n^2), т.к. делается линейный поиск минимума по массиву n раз
// по идее можно быстрее
// Алгоритм не будет работать, если в графе есть ребра отрицательной длины

std::vector<double> WeightedGraph::get_distances_Dijkstras_algorithm(unsigned vertice)
{
    size_t sz = adjacency_list.size();
    std::vector<double> d(vertices, std::numeric_limits<double>::infinity());
    d[vertice] = 0;
    std::vector<double> dist = d;
    std::set<unsigned> L;
    L.insert(vertice);
    unsigned last_added = vertice;
    for (int cycle = 0; cycle < sz; ++cycle)
    {
        for (int i = 0; i < adjacency_list[last_added].size(); ++i)
        {
            unsigned nbr = adjacency_list[last_added][i];
            // std::cout << d[last_added] << ' ' << edges_length[last_added][i] << ' ' << d[nbr] << '\n';
            if (d[last_added] + edges_length[last_added][i] < d[nbr] && L.find(nbr) == L.end())
            {
                d[nbr] = d[last_added] + edges_length[last_added][i] ;
            }
        }
        double _min_value = std::numeric_limits<double>::infinity();
        unsigned _min_index = 0;
        for (int i = 0; i < sz; ++i)
        {
            if (d[i] < _min_value && L.find(i) == L.end())
            {
                _min_value = d[i];
                _min_index = i;
            }
        }
        dist[_min_index] = d[_min_index];
        L.insert(_min_index);
        last_added = _min_index;
    }
    return dist;
}

/*
Алгоритм Форда-Белмана.
Работает для ОРГРАФОВ без циклов отрицательной суммы (для неорграфов с хоть одном отрицательным ребром не работает, т.к. это
ребро образует собой цикл отрицательной суммы, который можно проходить туда-сюда бесконечное кол-во раз).
Работает за O(|V|*|E|) по времени, O(|V|) по памяти
*/

std::vector<double> WeightedGraph::get_distances_Bellman_Ford_algorithm(unsigned vertice)
{
    // Алгоритм за О(n^2) памяти, по dp можно восстановить сам путь
    // std::vector<std::vector<double>> dp(vertices, std::vector<double>(vertices,
    //     std::numeric_limits<double>::infinity()));
    // dp[vertice][0] = 0;
    // for (unsigned k = 1; k < vertices - 1; ++k)
    // {
    //     for (unsigned u = 0; u < vertices; ++u)
    //     {
    //         for (unsigned _v = 0; _v < adjacency_list[u].size(); ++_v)
    //         {
    //             unsigned v = adjacency_list[u][_v];
    //             try
    //             {
    //                 dp[v][k] = std::min<double>(dp[v][k], dp[u][k - 1] + edges_length[u][_v]);
    //                 // dp.at(v).at(k) = std::min<double>(dp.at(v).at(k), dp.at(u).at(k - 1) + edges_length.at(u).at(_v));
    //             } catch (...) {
    //                 // std::cout << "exp " << u << ' ' << _v << ' ' << v << ' ' << k << '\n';
    //             }
    //             // std::cout << v << ' ' << k << ' ' << dp[v][k] << ' ' << dp[u][k - 1] + adjacency_list[u][v] << '\n';
    //         }
    //     }
    // }
    // // for (auto x : dp)
    // // {
    // //     for (auto y : x) std::cout << y << ' ';
    // //     std::cout << '\n';
    // // }
    // std::vector<double> res(vertices);
    // for (int i = 0; i < vertices; ++i)
    // {
    //     double _res = dp[i][0];
    //     for (int j = 1; j < vertices - 1; ++j)
    //     {
    //         if (_res > dp[i][j]) _res = dp[i][j];
    //     }
    //     res[i] = _res;
    // }
    // return res;
    std::vector<double> dp(vertices, std::numeric_limits<double>::infinity());
    std::vector<double> res(vertices, std::numeric_limits<double>::infinity());
    dp[vertice] = 0;
    for (int k = 1; k < vertices; ++k)
    {
        for (unsigned u = 0; u < vertices; ++u)
        {
            for (unsigned _v = 0; _v < adjacency_list[u].size(); ++_v)
            {
                unsigned v = adjacency_list[u][_v];
                dp[v] = std::min<double>(dp[v], dp[u] + edges_length[u][_v]);
                if (res[v] > dp[v]) res[v] = dp[v];
            }
        }
    }
    return res;
}

/*
Реализация алгоритма Флойда-Уоршелла. APSP - all pairs shortest paths. Работает за O(|V|^3) по времени, O(|V|^2) 
по памяти. Может принимать графы с ребрами отрицательной длины без циклов отрицательной суммы.
*/

std::vector<std::vector<double>> WeightedGraph::get_APSP()
{
    std::vector<std::vector<double>> dp(vertices, std::vector<double>(vertices, 
        std::numeric_limits<double>::infinity()));
    for (int i = 0; i < vertices; ++i) dp[i][i] = 0;
    for (int u = 0; u < vertices; ++u)
    {
        for (int _v = 0; _v < adjacency_list[u].size(); ++_v)
        {
            dp[u][adjacency_list[u][_v]] = edges_length[u][_v];
        }
    }
    for (int k = 1; k <= vertices; ++k)
    {
        for (int u = 0; u < vertices; ++u)
        {
            for (int v = 0; v < vertices; ++v)
            {
                dp[u][v] = std::min<double>(dp[u][v], dp[u][k - 1] + dp[k - 1][v]);
            }
        }
    }
    return dp;
}

// двусторонний алгоритм Дейкстры. Работает за O(|V| + |E|), в этой реализации за
// O(|V|^2) из-за линейного поиска минимума. Для возврата пути предыдущая вершина
// записывается в массив каждый раз при записи в массив d

std::pair<double, std::vector<unsigned>> WeightedGraph::get_path(
    unsigned v1,
    unsigned v2)
{
    if (v1 == v2) return std::pair<double, std::vector<unsigned>>(0, {v1});
    std::vector<double> d1(vertices, std::numeric_limits<double>::infinity());
    std::vector<double> d2 = d1;
    d1[v1] = 0;
    d2[v2] = 0;
    std::vector<double> dist1 = d1, dist2 = d2;
    std::set<unsigned> L1, L2;
    L1.insert(v1);
    L2.insert(v2);
    unsigned last_added1 = v1, last_added2 = v2;
    std::vector<int> ancestors1(vertices, -1);
    std::vector<int> ancestors2 = ancestors1;
    for (int cycle = 0; cycle < vertices; ++cycle)
    {
        for (int i = 0; i < adjacency_list[last_added1].size(); ++i)
        {
            unsigned nbr = adjacency_list[last_added1][i];
            if (d1[last_added1] + edges_length[last_added1][i] < d1[nbr] &&
                L1.find(nbr) == L1.end())
            {
                d1[nbr] = d1[last_added1] + edges_length[last_added1][i];
                ancestors1[nbr] = last_added1;
            }
        }
        for (int i = 0; i < adjacency_list[last_added2].size(); ++i)
        {
            unsigned nbr = adjacency_list[last_added2][i];
            if (d2[last_added2] + edges_length[last_added2][i] < d2[nbr] &&
                L2.find(nbr) == L2.end())
            {
                d2[nbr] = d2[last_added2] + edges_length[last_added2][i];
                ancestors2[nbr] = last_added2;
            }
        }
        double min_value1 = std::numeric_limits<double>::infinity();
        double min_value2 = min_value1;
        unsigned min_index1 = 0, min_index2 = 0;
        for (int i = 0; i < vertices; ++i)
        {
            if (d1[i] < min_value1 && L1.find(i) == L1.end())
            {
                min_value1 = d1[i];
                min_index1 = i;
            }
        }
        for (int i = 0; i < vertices; ++i)
        {
            if (d2[i] < min_value2 && L2.find(i) == L2.end())
            {
                min_value2 = d2[i];
                min_index2 = i;
            }
        }
        dist1[min_index1] = d1[min_index1];
        dist2[min_index2] = d2[min_index2];
        L1.insert(min_index1);
        L2.insert(min_index2);
        for (unsigned i = 0; i < vertices; ++i)
        {
            double inf = std::numeric_limits<double>::infinity();
            if (dist1[i] != inf && dist2[i] != inf)
            {
                // for (auto x : L1) std::cout << x << ' ';
                // std::cout << '\n';
                // for (auto x : L2) std::cout << x << ' ';
                // std::cout << '\n';
                double dist = dist1[i] + dist2[i];
                // std::cout << dist << '\n';
                std::deque<unsigned> path = {i};
                for (unsigned l1 : L1)
                {
                    for (unsigned l2 : L2)
                    {
                        int j = 0;
                        for (; j < adjacency_list[l1].size(); ++j)
                        {
                            if (adjacency_list[l1][j] == l2) break;
                        }
                        if (j == adjacency_list[l1].size()) continue;
                        if (dist1[l1] + edges_length[l1][j] + dist2[l2] < dist)
                        {
                            dist = dist1[l1] + edges_length[l1][j] + dist2[l2];
                            path = {l1, l2};
                            // std::cout << "a better path " << l1 << ' '
                            //     << l2 << '\n';
                        }
                    }
                }
                unsigned anc = ancestors1[path.front()];
                while (anc != -1) 
                {
                    path.push_front(anc);
                    anc = ancestors1[anc];
                }
                anc = ancestors2[path.back()];
                while (anc != -1)
                {
                    path.push_back(anc);
                    anc = ancestors2[anc];
                }
                std::vector<unsigned> res(path.begin(), path.end());
                return std::make_pair(dist, res);
            }
        }
        last_added1 = min_index1;
        last_added2 = min_index2;
    }
    return std::pair<double, std::vector<unsigned>>(
        std::numeric_limits<double>::infinity(), {});
}

// Дейкстра за O(|E|log|V|) за счет использования std::set вместо массива. 
// Еще есть вариант на std::priority_queue

std::vector<double> WeightedGraph::Dijkstra_on_set(unsigned v)
{
    auto cmp = [](auto& a, auto& b)
    {
        if (a.first == b.first) return a.second < b.second;
        return a.first < b.first;
    };
    std::set<std::pair<double, unsigned>, decltype(cmp)> s(cmp);
    std::vector<double> dist(vertices, 
        std::numeric_limits<double>::infinity());
    s.insert(std::make_pair(0, v));
    while (!s.empty())
    {
        auto p = s.begin();
        unsigned vertice = (*p).second;
        unsigned dist_tmp = (*p).first;
        if (dist[vertice] > dist_tmp) dist[vertice] = dist_tmp;
        s.erase(p);
        for (int i = 0; i < adjacency_list[vertice].size(); ++i)
        {
            unsigned nbr = adjacency_list[vertice][i];
            if (dist[nbr] > dist_tmp + edges_length[vertice][nbr])
            {
                s.insert(std::make_pair(dist_tmp + 
                    edges_length[vertice][i], nbr));
            }
        }
    }
    return dist;
}

int main()
{
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
    std::vector<std::vector<double>> edges_length = 
    {
    /*0*/ {3},
    /*1*/ {1, 4, 2},
    /*2*/ {1, 4, 3, 3},
    /*3*/ {4, 4, 1, 1, 3},
    /*4*/ {3, 5},
    /*5*/ {3, 1, 2},
    /*6*/ {2},
    /*7*/ {5, 1},
    /*8*/ {3, 1, 1, 2},
    /*9*/ {2, 3, 2}
    };
    std::vector<std::vector<unsigned>> adjacency_list_2 = adjacency_list;
    std::vector<std::vector<double>> edges_length_2 = 
    {
    /*0*/ {3},
    /*1*/ {1, -1, 2},
    /*2*/ {1, 2, 3, 3},
    /*3*/ {-1, 2, 1, 2, 3},
    /*4*/ {3, -3},
    /*5*/ {3, 1, -1},
    /*6*/ {-1},
    /*7*/ {-3, 1},
    /*8*/ {3, 2, 1, -2},
    /*9*/ {2, 3, -2}
    };
    std::vector<std::vector<unsigned>> adjacency_list3 = 
    {
        {1, 3},
        {0, 2},
        {1, 3},
        {0, 2},
    };
    std::vector<std::vector<double>> edges_length3 = 
    {
        {-1, -2},
        {1, 3},
        {1, 4},
        {2, 3},
    };
    std::vector<std::vector<unsigned>> adjacency_list_4 = 
    {
    /*0*/ {1, 2},
    /*1*/ {0, 2},
    /*2*/ {0, 1, 3},
    /*3*/ {2, 5, 6},
    /*4*/ {5},
    /*5*/ {3, 4},
    /*6*/ {3, 7, 9},
    /*7*/ {6, 10, 18},
    /*8*/ {9, 11},
    /*9*/ {6, 8, 10, 12},
    /*10*/{7, 9, 13},
    /*11*/{8, 12},
    /*12*/{9, 11, 13, 16},
    /*13*/{10, 12, 16, 17},
    /*14*/{15},
    /*15*/{14, 16},
    /*16*/{12, 13, 15, 17},
    /*17*/{13, 16, 29, 30},
    /*18*/{7, 19},
    /*19*/{18, 20, 21},
    /*20*/{19, 22},
    /*21*/{19, 22, 24},
    /*22*/{20, 21, 25},
    /*23*/{24, 26},
    /*24*/{21, 23, 25},
    /*25*/{22, 24, 27},
    /*26*/{23, 27},
    /*27*/{25, 26, 28, 29},
    /*28*/{27, 29},
    /*29*/{17, 27, 28, 30},
    /*30*/{17, 29}
    };
    std::vector<std::vector<double>> edges_length_4 = 
    {
    /*0*/ {2, 2.8284},
    /*1*/ {2, 2},
    /*2*/ {2.8284, 2, 6.0827},
    /*3*/ {6.0827, 1, 5},
    /*4*/ {5},
    /*5*/ {1, 5},
    /*6*/ {5, 3, 3},
    /*7*/ {3, 3, 3.6055},
    /*8*/ {3, 3},
    /*9*/ {3, 3, 3, 3},
    /*10*/{3, 3, 3},
    /*11*/{3, 3},
    /*12*/{3, 3, 3, 3},
    /*13*/{3, 3, 4.2426, 3},
    /*14*/{2.8284},
    /*15*/{2.8284, 5},
    /*16*/{3, 4.2426, 5, 3},
    /*17*/{3, 3, 8.2462, 6},
    /*18*/{3.6055, 4},
    /*19*/{4, 3, 3},
    /*20*/{3, 3},
    /*21*/{3, 3, 3},
    /*22*/{3, 3, 3},
    /*23*/{2, 5},
    /*24*/{3, 2, 3},
    /*25*/{3, 3, 5.8309},
    /*26*/{5, 2},
    /*27*/{5.8309, 2, 3, 3.6055},
    /*28*/{3, 3.1622},
    /*29*/{8.2462, 3.6055, 3.1622, 8.9442},
    /*30*/{6, 8.9442}
    };
    std::vector<std::vector<unsigned>> adjacency_list_5 = 
    {
        {1, 2},
        {0, 2},
        {0, 1},
    };
    std::vector<std::vector<double>> edges_length_5 =
    {
        {2, 2.2824},
        {2, 2},
        {2.2824, 2}
    };
    WeightedGraph g(adjacency_list, edges_length);
    WeightedGraph g2(adjacency_list_2, edges_length_2);
    WeightedGraph g3(adjacency_list3, edges_length3);
    Graph not_weighted(adjacency_list);
    WeightedGraph g4(adjacency_list_4, edges_length_4);
    WeightedGraph g5(adjacency_list_5, edges_length_5);
    // std::vector<std::string> to_file;
    // for (int i = 0; i < g4.size(); ++i)
    // {
    //     for (int j = i + 1; j < g4.size(); ++j)
    //     {
    //         auto x = g4.get_path(i, j);
    //         to_file.push_back(std::to_string(i));
    //         to_file.push_back(" ");
    //         to_file.push_back(std::to_string(j));
    //         to_file.push_back(" ");
    //         to_file.push_back(std::to_string(static_cast<int>(x.first * 1000)));
    //         to_file.push_back("\n");
    //     }
    // }
    // setlocale(LC_ALL, "Rus");
    // std::ofstream fout;
    // fout.open("C:/c++/ne_planar.txt");
    // if (!fout.is_open()) std::cout << "not open";
    // for (auto str : to_file) fout << str;
    // fout.close();
    std::cout << 2 << '\n';
    auto p = g4.get_path(19, 29);
    for (auto x : p.second) std::cout << x << ' ';
}
