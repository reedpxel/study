#include <iostream>
#include <vector>

class FenwickTree
{
    std::vector<int> v;
    std::vector<int> tree;
    int F(int i) const;
    int G(int i) const;
    std::vector<int> build_fenwick_tree();
    int prefix_sum(int i) const;
    void update_delta(int index, int delta);
public:
    FenwickTree() = delete;
    FenwickTree(const std::vector<int>& v);
    void print_vector() const;
    void print_fenwick_tree() const;
    int get_sum(int l, int r) const;
    void update(int index, int val);
};

int FenwickTree::F(int i) const { return i & i + 1; }

int FenwickTree::G(int i) const { return i | i + 1; }

std::vector<int> FenwickTree::build_fenwick_tree()
{
    std::vector<int> tree;
    std::vector<int> _prefix_sum;
    _prefix_sum.push_back(v[0]);
    for (int i = 1; i < v.size(); ++i)
    {
        _prefix_sum.push_back(_prefix_sum[i - 1] + v[i]);
    }
    for (int i = 0; i < v.size(); ++i)
    {
        int ind = F(i) - 1;
        if (ind < 0) tree.push_back(_prefix_sum[i]);
        else tree.push_back(_prefix_sum[i] - _prefix_sum[F(i) - 1]);
    }
    return tree;
}

int FenwickTree::prefix_sum(int i) const
{
    int ans = 0;
    for (; i > 0; i = F(i) - 1)
    {
        ans += tree[i];
    }
    return ans;
}

void FenwickTree::update_delta(int index, int delta)
{
    for (; index < tree.size(); index = G(index))
    {
        tree[index] += delta;
    }
}

FenwickTree::FenwickTree(const std::vector<int>& v) : v(std::move(v)), tree(std::move(build_fenwick_tree())) {}

void FenwickTree::print_vector() const
{
    for (auto x : v) std::cout << x << ' ';
    std::cout << '\n';
}

void FenwickTree::print_fenwick_tree() const
{
    for (auto x : tree) std::cout << x << ' ';
    std::cout << '\n';
}

int FenwickTree::get_sum(int l, int r) const { return prefix_sum(r) - prefix_sum(l - 1); }


void FenwickTree::update(int index, int val)
{
    update_delta(index, val - v[index]);
    v[index] = val;
}

int main()
{
    std::vector<int> v;
    for (int i = 0; i < 1000; ++i) v.push_back(i);
    FenwickTree t(v);
    t.update(1, 10);
    t.print_vector();
    t.print_fenwick_tree();
    std::cout << t.get_sum(0, v.size() - 1) << '\n';
}