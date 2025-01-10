#include <iostream>
#include <vector>
#include <cmath>

class SegmentTree
{
    int heapSize;
    int dataSize;
    int* heap;

    int parent(int i) { return (i - 1) >> 1; }
    int leftChild(int i) { return 2 * i + 1; }
    int rightChild(int i) { return 2 * i + 2; }
    int sumRangeRec(int left, int right, int index)
    {
        int _l = index;
        int _r = index;
        for (; _l < heapSize >> 1; _l = leftChild(_l));
        for (; _r < heapSize >> 1; _r = rightChild(_r));
        _l -= heapSize >> 1;
        _r -= heapSize >> 1;
        if (_r < left || _l > right) return 0;
        else if (left <= _l && _r <= right) return heap[index];
        else return sumRangeRec(left, right, leftChild(index)) + 
            sumRangeRec(left, right, rightChild(index));
    }
public:
    SegmentTree(const std::vector<int>& v) :
        heapSize(2 * pow(2, ceil(log2(v.size()))) - 1),
        dataSize(v.size()),
        heap(new int[heapSize]())
    {

        for (int i = heapSize >> 1; i < (heapSize >> 1) + dataSize; ++i)
        {
            heap[i] = v[i - (heapSize >> 1)];
        }
        for (int i = (heapSize >> 1) - 1; i >= 0; --i)
        {
            heap[i] = heap[leftChild(i)] + heap[rightChild(i)];
        }
    }
    void update(int index, int value)
    {
        index += heapSize >> 1;
        int diff = value - heap[index];
        for (; index > 0; index = parent(index))
        {
            heap[index] += diff;
        }
        heap[0] += diff;
    }
    int sumRange(int left, int right)
    {
        return sumRangeRec(left, right, 0);
    }
    ~SegmentTree() { delete heap; }
};

int main()
{
    SegmentTree t({3, 1, 2, 4, -1});
    std::cout << t.sumRange(3, 4);
}