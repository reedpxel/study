#include <iostream>
#include <vector>
#include <deque>
#include <queue>
#include <cmath>
#include <limits>
#include <set>
#include <fstream>

class PlanarGraph
{
public:
    struct Vertice
    {
        int x;
        int y;
    };
private:
    std::vector<Vertice> vertices;
    std::vector<std::vector<unsigned>> adjacency_list;

    double edge_length(unsigned v_ind1, unsigned v_ind2);
    double euristic_func(unsigned v_ind1, unsigned v_ind2);
public:
    PlanarGraph(const std::vector<Vertice>& vertices,
        const std::vector<std::vector<unsigned>>& adjacency_list);
    size_t size();
    std::vector<unsigned> get_path_bidirectional_Dijkstra(
        unsigned v1, unsigned v2, double& path_length_buffer);
    std::vector<unsigned> get_path_a_star(
        unsigned v1, unsigned v2, double& path_length_buffer);
};

PlanarGraph::PlanarGraph(const std::vector<Vertice>& vertices,
    const std::vector<std::vector<unsigned>>& adjacency_list) :
    vertices(vertices),
    adjacency_list(adjacency_list) {}

double PlanarGraph::edge_length(unsigned v_ind1, unsigned v_ind2)
{
    Vertice v1 = vertices[v_ind1], v2 = vertices[v_ind2];
    double dx = v2.x - v1.x;
    double dy = v2.y - v1.y;
    return sqrt(dx * dx + dy * dy);
}

double PlanarGraph::euristic_func(unsigned v_ind1, unsigned v_ind2)
{
    Vertice v1 = vertices[v_ind1], v2 = vertices[v_ind2];
    double dx = v2.x - v1.x;
    double dy = v2.y - v1.y;
    return dx * dx + dy * dy;
}

size_t PlanarGraph::size() { return vertices.size(); }

// на массиве => работает за O(|V|^2). Проверен в сравнении с алгоритмом из
// shortest_path_search_algorithms.cpp

std::vector<unsigned> PlanarGraph::get_path_bidirectional_Dijkstra(
    unsigned v1, unsigned v2, double& path_length_buffer)
{
    if (v1 == v2)
    {
        path_length_buffer = 0;
        return {v1};
    }
    double inf = std::numeric_limits<double>::infinity();
    std::vector<double> d1(size(), inf), d2 = d1;
    d1[v1] = 0;
    d2[v2] = 0;
    std::vector<double> dist1 = d1, dist2 = d2;
    std::vector<int> ancestors1(size(), -1), ancestors2 = ancestors1;
    std::set<unsigned> L1, L2;
    L1.insert(v1);
    L2.insert(v2);
    unsigned last_added1 = v1, last_added2 = v2;
    for (int cycle = 0; cycle < size(); ++cycle)
    {
        for (int i = 0; i < adjacency_list[last_added1].size(); ++i)
        {
            unsigned nbr = adjacency_list[last_added1][i];
            if (d1[nbr] < d1[last_added1] + edge_length(last_added1, nbr) ||
                L1.find(nbr) != L1.end()) continue;
            d1[nbr] = d1[last_added1] + edge_length(last_added1, nbr);
            ancestors1[nbr] = last_added1;
        }
        for (int i = 0; i < adjacency_list[last_added2].size(); ++i)
        {
            unsigned nbr = adjacency_list[last_added2][i];
            if (d2[nbr] < d2[last_added2] + edge_length(last_added2, nbr) ||
                L2.find(nbr) != L2.end()) continue;
            d2[nbr] = d2[last_added2] + edge_length(last_added2, nbr);
            ancestors2[nbr] = last_added2;
        }
        double min_value1 = inf, min_value2 = inf;
        unsigned min_index1 = 0, min_index2 = 0;
        for (int i = 0; i < size(); ++i)
        {
            if (d1[i] < min_value1 && L1.find(i) == L1.end())
            {
                min_value1 = d1[i];
                min_index1 = i;
            }
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
        last_added1 = min_index1;
        last_added2 = min_index2;
        for (unsigned i = 0; i < size(); ++i)
        {
            if (dist1[i] != inf && dist2[i] != inf)
            {
                path_length_buffer = dist1[i] + dist2[i];
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
                        if (dist1[l1] + edge_length(l1, l2) + 
                            dist2[l2] < path_length_buffer)
                        {
                            path_length_buffer = dist1[l1] + 
                                edge_length(l1, l2) + dist2[l2];
                            path = {l1, l2};
                        }
                    }
                }
                unsigned v_tmp = ancestors1[path.front()];
                while (v_tmp != -1)
                {
                    path.push_front(v_tmp);
                    v_tmp = ancestors1[v_tmp];
                }
                v_tmp = ancestors2[path.back()];
                while (v_tmp != -1)
                {
                    path.push_back(v_tmp);
                    v_tmp = ancestors2[v_tmp];
                }
                std::vector<unsigned> res(path.begin(), path.end());
                return res;
            }
        }
    }
    return {};
}

// A* на массиве. Работает за О(|V|^2), но быстрее Дейкстры, т.к.
// оптимальная вершина движется в сторону ищущейся

std::vector<unsigned> PlanarGraph::get_path_a_star(
    unsigned v1, unsigned v2, double& path_length_buffer)
{
    if (v1 == v2) 
    {
        path_length_buffer = 0;
        return {v1};
    }
    double inf = std::numeric_limits<double>::infinity();
    std::vector<double> dist(size(), inf), h = dist;
    dist[v1] = 0;
    h[v1] = edge_length(v1, v2);
    std::vector<int> anc(size(), -1);
    unsigned last_added = v1;
    std::set<unsigned> L;
    L.insert(v1);
    for (int cycle = 0; cycle < size(); ++cycle)
    {
        // std::cout << last_added << ' ';
        for (int i = 0; i < adjacency_list[last_added].size(); ++i)
        {            
            unsigned nbr = adjacency_list[last_added][i];
            // std::cout << nbr << ' ';
            if (nbr == v2)
            {
                path_length_buffer = dist[last_added] + 
                    edge_length(last_added, v2);
                std::deque<unsigned> path = {last_added, v2};
                unsigned vertice_tmp = anc[last_added];
                while (vertice_tmp != -1)
                {
                    path.push_front(vertice_tmp);
                    vertice_tmp = anc[vertice_tmp];
                }
                // for (auto x : dist) std::cout << x << ' ';
                // std::cout << '\n';
                return std::vector(path.begin(), path.end());
            }
            if (h[nbr] > dist[last_added] + edge_length(last_added, nbr) +
                    edge_length(nbr, v2) && L.find(nbr) == L.end())
            {
                h[nbr] = dist[last_added] + edge_length(last_added, nbr) +
                    edge_length(nbr, v2);
                anc[nbr] = last_added;
            }
            // std::cout << last_added << ' ' << nbr << '\n';
            // for (auto x : h) std::cout << x << ' ';
            // std::cout << '\n';
            // for (auto x : dist) std::cout << x << ' ';
            // std::cout << '\n';
        }
        // std::cout << '\n';
        double min_h_value = inf;
        unsigned min_h_index = 0;
        for (int i = 0; i < size(); ++i)
        {
            if (min_h_value > h[i]
                && L.find(i) == L.end())
            {
                min_h_value = h[i];
                min_h_index = i;
            }
        }
        dist[min_h_index] = h[min_h_index] - edge_length(min_h_index, v2);
        last_added = min_h_index;
        L.insert(last_added);
    }
    // for (auto x : h) std::cout << x << ' ';
    // std::cout << '\n';
    return {};
}

int main()
{
    std::vector<PlanarGraph::Vertice> vertices = 
    {
    /*0*/ {10, 24},
    /*1*/ {10, 22},
    /*2*/ {8, 22},
    /*3*/ {7, 16},
    /*4*/ {2, 15},
    /*5*/ {7, 15},
    /*6*/ {12, 16},
    /*7*/ {15, 16},
    /*8*/ {9, 13},
    /*9*/ {12, 13},
    /*10*/{15, 13},
    /*11*/{9, 10},
    /*12*/{12, 10},
    /*13*/{15, 10},
    /*14*/{5, 5},
    /*15*/{7, 7},
    /*16*/{12, 7},
    /*17*/{15, 7},
    /*18*/{17, 19},
    /*19*/{21, 19},
    /*20*/{24, 19},
    /*21*/{21, 16},
    /*22*/{24, 16},
    /*23*/{19, 13},
    /*24*/{21, 13},
    /*25*/{24, 13},
    /*26*/{19, 8},
    /*27*/{21, 8},
    /*28*/{24, 8},
    /*29*/{23, 5},
    /*30*/{15, 1}    
    };
    std::vector<std::vector<unsigned>> adjacency_list = 
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
    PlanarGraph pg(vertices, adjacency_list);
    double dist = 0;
    // for (int i = 0; i < pg.size(); ++i)
    // {
    //     for (int j = 0; j < pg.size(); ++j)
    //     {
    //         double dist1, dist2;
    //         pg.get_path_a_star(i, j, dist1);
    //         pg.get_path_bidirectional_Dijkstra(i, j, dist2);
    //         if (dist1 - dist2 > 0.001) std::cout << i << ' ' << j << ' ' << dist1 <<
    //             ' ' << dist2 << '\n';
    //     }
    // }
    auto x = pg.get_path_a_star(0, 28, dist);
    std::cout << dist << '\n';
    for (auto xx : x) std::cout << xx << ' ';
}
