#include <iostream>
#include <vector>
#include <algorithm>
#include <stack>

// решение задачи о рюкзаке. Сложность O(N * f(W)), где N - кол-во предметов, W - вместимость рюкзака, f(W) - кол-во рекурсивных вызовов dp в зависимости
// от размера рюкзака и предметов. f является показательной функцией, но сложность алгоритма быстрее, чем перебор (2^n)

// TO DO: как возвращать сами предметы, а не макс стоимость?

class Item : public std::pair<int, int>
{
public:
    int& weight = first;
    int& price = second;
    Item(int _weight, int _price) : std::pair<int, int>(_weight, _price) {}
};

int dp(std::vector<Item>& v, int i, int w)
{
    if (i < 0 || w <= 0) return 0;
    if (w < v[i].weight) return dp(v, i - 1, w);
    return std::max<int>(dp(v, i - 1, w), dp(v, i - 1, w - v[i].weight) + v[i].price);
}

int get_price(std::vector<Item>& v, int w) { return dp(v, v.size() - 1, w); }

// int dp_items(std::vector<Item>& v, std::vector<bool>& is_in_knapsack, int i, int w, int prev_i)
// {
    
//     if (i < 0 || w <= 0) return 0;
//     if (w < v[i].weight) return dp_items(v, is_in_knapsack, i - 1, w, prev_i);
//     int i1 = dp_items(v, is_in_knapsack, i - 1, w, prev_i);
//     int i2 = dp_items(v, is_in_knapsack, i - 1, w - v[i].weight, prev_i) + v[i].price;
//     if (i1 > i2) return i1;
//     if (prev_i != v.size()) is_in_knapsack[prev_i] = true;
//     prev_i = i;
//     return i2;
//     // return std::max<int>(dp_items(v, is_in_knapsack, i - 1, w), dp_items(v, is_in_knapsack, i - 1, w - v[i].weight) + v[i].price);
// }

// std::vector<Item> get_items(std::vector<Item>& v, int w)
// {
//     std::vector<bool> is_in_knapsack(v.size(), false);
//     int ans = dp_items(v, is_in_knapsack, v.size() - 1, w, v.size());
//     for (auto x : is_in_knapsack) std::cout << x;
//     std::cout << '\n' << ans << '\n';
//     std::vector<Item> res;
//     for (int i = 0; i < is_in_knapsack.size(); ++i)
//     {
//         if (is_in_knapsack[i]) res.push_back(v[i]);
//     }
//     return res;
// }

int main()
{

}
