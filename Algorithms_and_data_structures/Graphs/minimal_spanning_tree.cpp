#include <iostream>
#include <vector>
#include <set>
#include <limits>
#include <algorithm>
#include <stack>
#include <cmath>

class Tree;

class Graph
{
protected:
    std::vector<std::vector<unsigned>> adjacency_list;
    std::vector<std::vector<double>> edges_length;
    struct Edge
    {
        unsigned v1;
        unsigned v2;
        double edge_length;
        Edge(unsigned v1, unsigned v2, double edge_length);
    };

public:

    Graph(const std::vector<std::vector<unsigned>>& adjacency_list,
        const std::vector<std::vector<double>>& edges_length);
    unsigned size();
    Tree get_mst_as_graph(unsigned root);
    std::vector<std::pair<unsigned, unsigned>> get_mst_Prima_algorithm();
    std::vector<std::pair<unsigned, unsigned>> get_mst_Kruskal_algorithm();
};

class Tree : public Graph
{
    unsigned root;

    unsigned ancestor(unsigned v, const std::vector<unsigned>& depth);
    std::vector<unsigned> dfs_for_dca();
    void get_vectors_for_dca(std::vector<unsigned>& ett,
        std::vector<unsigned>& depth,
        std::vector<unsigned>& first);

public:
    Tree(const std::vector<std::vector<unsigned>>& adjacency_list,
        const std::vector<std::vector<double>>& edges_length,
        unsigned root);
    unsigned get_LCA_naive(unsigned u, unsigned v);
    unsigned get_LCA_binary_rise(unsigned u, unsigned v);
    unsigned get_LCA_ETT(unsigned u, unsigned v);
    std::vector<std::pair<unsigned, unsigned>> 
        get_mst_Prima_algorithm() = delete;
    std::vector<std::pair<unsigned, unsigned>> 
        get_mst_Kruskal_algorithm() = delete;
};

class DSU // disjoint set union, где индекс элемента равен элементу
{
public:
    std::vector<unsigned> unions;
    unsigned amount_of_unions;
public:
    DSU(unsigned _sz);
    unsigned operator[](unsigned i);
    unsigned size();
    void unite(unsigned a, unsigned b); // за O(sz)
    bool is_same(unsigned a, unsigned b); // за О(1)
};

Graph::Edge::Edge(unsigned v1, unsigned v2, double edge_length) :
    v1(v1),
    v2(v2),
    edge_length(edge_length) {}

DSU::DSU(unsigned _sz) : unions(std::vector<unsigned>(_sz)), amount_of_unions(_sz)
{
    for (unsigned i = 0; i < unions.size(); ++i) unions[i] = i;
}

unsigned DSU::operator[](unsigned i) { return i; }

unsigned DSU::size() { return amount_of_unions; }

void DSU::unite(unsigned a, unsigned b)
{
    if (unions[a] == unions[b]) return;
    unsigned union_b = unions[b];
    for (int i = 0; i < unions.size(); ++i)
    {
        if (unions[i] == union_b)
        {
            unions[i] = unions[a];
        }
    }
    --amount_of_unions;
}

bool DSU::is_same(unsigned a, unsigned b) { return unions[a] == unions[b]; }

Graph::Graph(const std::vector<std::vector<unsigned>>& adjacency_list,
    const std::vector<std::vector<double>>& edges_length) :
    adjacency_list(adjacency_list), 
    edges_length(edges_length) {}

Tree::Tree(const std::vector<std::vector<unsigned>>& adjacency_list,
    const std::vector<std::vector<double>>& edges_length,
    unsigned root) : 
    Graph(adjacency_list, edges_length),
    root(root) {}

unsigned Graph::size() { return adjacency_list.size(); }

// этот алгоритм можно написать на std::priority_queue за O(|E|log|V|), 
// эта версия за O(|V|^2)

std::vector<std::pair<unsigned, unsigned>> Graph::get_mst_Prima_algorithm()
{
    if (!size()) return {};
    std::vector<std::pair<unsigned, unsigned>> res;
    std::vector<bool> used(size(), false);
    used[0] = true;
    int used_vertices = 1;
    while (used_vertices < size())
    {
        double min_length = std::numeric_limits<double>::infinity();
        std::pair<unsigned, unsigned> min_edge(0, 0);
        unsigned vertice2 = 0;
        for (unsigned vertice1 = 0; vertice1 < size(); ++vertice1)
        {
            if (!used[vertice1]) continue;
            for (int i = 0; i < adjacency_list[vertice1].size(); ++i)
            {
                vertice2 = adjacency_list[vertice1][i];
                if (used[vertice2]) continue;
                if (edges_length[vertice1][i] < min_length)
                {
                    min_edge = {vertice1, vertice2};
                    min_length = edges_length[vertice1][i];
                }
            }
        }
        used[min_edge.second] = true;
        res.push_back(min_edge);
        ++used_vertices;
    }
    std::sort(res.begin(), res.end(), [](auto a, auto b){ return a.first < b.first; });
    return res;
}

// работает за O(|E|log|E|)
std::vector<std::pair<unsigned, unsigned>> Graph::get_mst_Kruskal_algorithm()
{
    if (!size()) return {};
    DSU dsu(size()); // каждый эл-т снм - вершина графа
    std::vector<Edge> edges;
    std::vector<std::vector<bool>> used(size(), std::vector<bool>(size(), false));
    std::vector<std::pair<unsigned, unsigned>> res;
    for (int i = 0; i < size(); ++i)
    {
        for (int j = 0; j < adjacency_list[i].size(); ++j)
        {
            unsigned nbr = adjacency_list[i][j];
            if (!used[i][nbr])
            {
                edges.push_back(Edge(i, nbr, edges_length[i][j]));
                used[nbr][i] = true;
            }
        }
    }
    std::sort(edges.begin(), edges.end(),
        [](Edge a, Edge b){ return a.edge_length < b.edge_length; });
    unsigned _i = 0;
    while (dsu.size() > 1)
    {
        if (dsu.is_same(edges[_i].v1, edges[_i].v2))
        {
            ++_i;
            continue;
        } else {
            res.push_back(std::make_pair(edges[_i].v1, edges[_i].v2));
            dsu.unite(edges[_i].v1, edges[_i].v2);
            ++_i;
        }
        if (_i > size()) _i = 0;
    }
    std::sort(res.begin(), res.end(), [](auto a, auto b){ return a.first < b.first; });
    return res;
}

Tree Graph::get_mst_as_graph(unsigned root)
{
    auto edges = get_mst_Prima_algorithm();
    std::vector<std::vector<unsigned>> new_adjacency_list(edges_length.size());
    for (auto _pair : edges)
    {
        new_adjacency_list[_pair.first].push_back(_pair.second);
        new_adjacency_list[_pair.second].push_back(_pair.first);
    }
    for (auto& v : new_adjacency_list)
    {
        std::sort(v.begin(), v.end());
    }
    std::vector<std::vector<double>> new_edges_length(edges_length.size());
    for (int i = 0; i < edges_length.size(); ++i)
    {
        new_edges_length[i].resize(new_adjacency_list[i].size());
    }
    for (int i = 0; i < new_edges_length.size(); ++i)
    {
        for (int j = 0; j < new_edges_length[i].size(); ++j)
        {
            unsigned ind = 0;
            for (; ind < edges_length[i].size(); ++ind)
            {
                if (adjacency_list[i][ind] == new_adjacency_list[i][j]) break;
            }
            new_edges_length[i][j] = edges_length[i][ind];
        }
    }
    return Tree(new_adjacency_list, new_edges_length, root);
}

// предподсчет - DFS (за O(|V|)), ответ на запрос за за O(|V|)
unsigned Tree::get_LCA_naive(unsigned u, unsigned v)
{
    std::vector<unsigned> depth = dfs_for_dca();
    bool v_less_u = depth[v] <= depth[u];
    unsigned& _u = v_less_u ? u : v;
    unsigned& _v = v_less_u ? v : u;
    while (depth[_v] != depth[_u]) _u = ancestor(_u, depth); 
    while (_u != _v)
    {
        _u = ancestor(_u, depth);
        _v = ancestor(_v, depth);
    }
    return _u;
}

unsigned Tree::ancestor(unsigned v, const std::vector<unsigned>& depth)
{
    if (root == v) return root;
    for (int i = 0; i < adjacency_list[v].size(); ++i)
    {
        unsigned nbr = adjacency_list[v][i];
        if (depth[nbr] < depth[v]) return nbr;
    }
    return size();
}

// метод бинарных подъемов, часть алгоритма работает за O(log|V|).
// Мб я что-то не понял, но на самом деле там вставка
// за О(|V|)
unsigned Tree::get_LCA_binary_rise(unsigned u, unsigned v)
{
    if (u == v) return u;
    // предподсчет за O(|V|log|V|)
    unsigned k = ceil(log2(size()));
    // std::cout << k << ' ';
    std::vector<std::vector<unsigned>> dp(size(),
        std::vector<unsigned>(k));
    std::vector<unsigned> depth = dfs_for_dca();
    bool v_less_u = depth[u] > depth[v];
    unsigned& _u = v_less_u ? u : v;
    unsigned& _v = v_less_u ? v : u;
    for (int i = 0; i < size(); ++i)
    {
        dp[i][0] = ancestor(i, depth);
    }
    for (int i = 1; i < k; ++i)
    {
        for (int j = 0; j < size(); ++j)
        {
            dp[j][i] = dp[dp[j][i - 1]][i - 1];
        }
    }
    // ответ на запрос за O(log|V|)
    for (int i = k - 1; i >= 0; --i)
    {
        int delta = depth[_u] - depth[_v];
        if (delta & (1 << i))
        {
            _u = dp[_u][i];
        }
    }
    if (_u == _v) return _u;
    unsigned i = 0;
    while (_u != _v)
    {
        _u = ancestor(_u, depth);
        _v = ancestor(_v, depth);
    }
    return _u;
}

// за O(|V| + |E|), обычный dfs на графе по списку смежности,
// возвращает глубины вершин
std::vector<unsigned> Tree::dfs_for_dca() 
{
    std::vector<unsigned> depth(size());
    unsigned depth_tmp = 0;
    std::stack<int> stack;
    stack.push(root);
    std::vector<bool> is_grey(size(), false);
    std::vector<bool> is_black(size(), false);
    while (!stack.empty())
    {
        unsigned cur = stack.top();
        if (!is_grey[cur])
        {
            is_grey[cur] = true;
            ++depth_tmp;
            for (unsigned i = 0; i < adjacency_list[cur].size(); ++i)
            {
                unsigned nbr = adjacency_list[cur][i];
                if (!is_grey[nbr])
                {
                    depth[nbr] = depth_tmp;
                    stack.push(nbr);
                }
            }
        } else if (!is_black[cur]) {
            is_black[cur] = true;
            stack.pop();
            --depth_tmp;
        } else {
            stack.pop();
            --depth_tmp;
        }
    }
    return depth;
}

// O(|V|log|V|) предподсчет, O(1) ответ на запрос,
unsigned Tree::get_LCA_ETT(unsigned u, unsigned v)
{
    if (u == v) return u;
    // предподсчет
    std::vector<unsigned> ett, depth, first;
    get_vectors_for_dca(ett, depth, first);
    std::vector<std::vector<unsigned>> sparsed_table(1, std::vector<unsigned>(depth.size()));
    for (unsigned i = 0; i < depth.size(); ++i) sparsed_table[0][i] = i;
    unsigned log_sz = log2(depth.size());
    for (int i = 1; i < log_sz + 1; ++i)
    {
        int tmp_sz = depth.size() - (1 << i) + 1;
        std::vector<unsigned> tmp(tmp_sz);
        for (int j = 0; j < tmp_sz; ++j)
        {
            tmp[j] = depth[sparsed_table[i - 1][j]] < 
                depth[sparsed_table[i - 1][j + (1 << (i - 1))]] ? 
                sparsed_table[i - 1][j] : 
                sparsed_table[i - 1][j + (1 << (i - 1))];
        }
        sparsed_table.emplace_back(tmp);
    }
    // получение ответа на запрос
    unsigned first_u = first[u];
    unsigned first_v = first[v];
    if (first_u > first_v) std::swap(first_u, first_v);
    unsigned k = log2(first_v - first_u);
    if (depth[sparsed_table[k][first_u]] < depth[sparsed_table[k][first_v - (1 << k) + 1]])
    {
        return ett[sparsed_table[k][first_u]];
    }
    return ett[sparsed_table[k][first_v - (1 << k) + 1]];
}

void Tree::get_vectors_for_dca(std::vector<unsigned>& ett,
    std::vector<unsigned>& depth,
    std::vector<unsigned>& first)
{
    // depth
    std::vector<unsigned> _depth(size());
    unsigned depth_tmp = 0;
    // ett
    std::vector<bool> used(size(), false);
    ett.reserve(2 * size() - 1);
    // first
    first.resize(size());
    int count = -1;
    // dfs
    std::stack<int> stack;
    stack.push(root);
    std::vector<bool> is_grey(size(), false);
    std::vector<bool> is_black(size(), false);
    while (!stack.empty())
    {
        ++count;
        unsigned cur = stack.top();
        if (!is_grey[cur])
        {
            first[cur] = count;
            is_grey[cur] = true;
            ++depth_tmp;
            for (int i = adjacency_list[cur].size() - 1; i >= 0; --i)
            {
                unsigned nbr = adjacency_list[cur][i];
                if (!is_grey[nbr])
                {
                    _depth[nbr] = depth_tmp;
                    stack.push(nbr);
                }
            }
        } else if (!is_black[cur]) {
            is_black[cur] = true;
            --depth_tmp;
            stack.pop();
        } else {
            --depth_tmp;
            stack.pop();
        }
    }
    unsigned cur = root;
    for (int cycle = 0; cycle < 2 * size() - 1; ++cycle)
    {
        ett.push_back(cur);
        used[cur] = true;
        bool no_children = true;
        for (int i = 0; i < adjacency_list[cur].size(); ++i)
        {
            unsigned nbr = adjacency_list[cur][i];
            if (!used[nbr])
            {
                cur = nbr;
                no_children = false;
                break;
            }
        }
        if (!no_children) continue;
        for (int i = 0; i < adjacency_list[cur].size(); ++i)
        {
            unsigned nbr = adjacency_list[cur][i];
            if (_depth[cur] - _depth[nbr] == 1)
            {
                cur = nbr;
                break;
            }
        }
    }
    depth.resize(ett.size());
    for (int i = 0; i < depth.size(); ++i)
    {
        depth[i] = _depth[ett[i]];
    }
}

int main()
{
    std::vector<std::vector<unsigned>> adj_list = 
    {
        {1, 2, 3},
        {0, 2, 4},
        {0, 1, 4, 6},
        {0},
        {1, 2, 5 ,6},
        {4, 6},
        {2, 4, 5}
    };
    std::vector<std::vector<double>> edges_length = 
    {
        {5, 3, 2},
        {5, 10, -10},
        {3, 10, 4, 2},
        {2},
        {-10, 4, 8, 7},
        {8, 3},
        {2, 7, 3}
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
    Graph g(adj_list, edges_length);
    Graph g2(adjacency_list_4, edges_length_4);
    Tree tree = g2.get_mst_as_graph(13);
    for (int i = 0; i < tree.size(); ++i)
    {
        for (int j = i; j < tree.size(); ++j)
        {
            if (tree.get_LCA_naive(i, j) == tree.get_LCA_ETT(i, j)) continue;
            std::cout << i << ' ' << j << ' ' << tree.get_LCA_ETT(i, j) << 
                ' ' << tree.get_LCA_naive(i, j) << '\n';
        }
    }
}
