#include <iostream>
#include <vector>

void swap(std::vector<int>& v, int p1, int p2)
{
    int tmp = v[p1];
    v[p1] = v[p2];
    v[p2] = tmp;
}

void sort_array_of_0_and_1(std::vector<int>& array)
{
    int p1 = 0;
    int p2 = array.size() - 1;
    while (p1 < p2)
    {
        while (!array[p1] && p1 < p2) ++p1;
        while (array[p2] && p1 < p2) --p2;
        if (p1 < p2) swap(array, p1, p2);
    }
}

void sort_array_of_0_and_1(std::vector<int>::iterator begin_, 
    std::vector<int>::iterator end_)
{
    while (begin_ < end_)
    {
        while (!*begin_ && begin_ < end_) ++begin_;
        while (*end_ && begin_ < end_) --end_;
        if (begin_ < end_) std::swap(*begin_, *end_);
    }
}

void sort_array_of_0_1_and_2(std::vector<int>::iterator begin_,
    std::vector<int>::iterator end_)
{
    std::vector<int>::iterator low = begin_;
    std::vector<int>::iterator middle = begin_;
    std::vector<int>::iterator high = end_ - 1;
    while (middle <= high)
    {
        switch (*middle)
        {
            case 0:
                std::swap(*low, *middle);
                ++low;
                ++middle;
            break;
            case 1:
                ++middle;
            break;
            case 2:
                std::swap(*middle, *high);
                --high;
        }
    }
}

int main()
{
    std::vector<int> v = {1, 2, 0, 1, 2, 2, 0, 0, 2, 1, 0, 1, 1, 0};
    sort_array_of_0_1_and_2(v.begin(), v.end());
    for (int x : v) std::cout << x << ' ';
    std::cout << '\n';
}
