#include <iostream>
#include <ctime>
#include <random>

void Swap(int& a, int& b){
    int t = a;
    a = b;
    b = t;
}

int RandomPartition(int* arr, int start, int end){
    std::random_device rand;
    int pivot_index = rand()%(end-start+1);
    Swap(arr[pivot_index], arr[start]);
    int start_index = start;
    for (int i=start; i<=end; ++i){
        if (arr[i]<arr[start_index]){
            Swap(arr[start_index+1], arr[i]);
            Swap(arr[start_index], arr[start_index+1]);
            ++start_index;
        }
    }
    return start_index;
}

int PartitionEnd(int* array, int start, int end){ //Lomuto`s scheme
    int pivot = array[end];
    int start_index = start;
    for (int i=start; i<end; ++i){
        if (array[i]<pivot){
            Swap(array[i], array[start_index]);
            ++start_index;
        }
    }
    Swap(array[end], array[start_index]);
    return start_index;
}

int QuickSelect(int* arr, int k, int begin, int end){
    int pivot = PartitionEnd(arr, begin, end);
    if (pivot-begin == k)
        return arr[pivot];
    if (pivot-begin>k)
        return QuickSelect(arr, k, begin, pivot-1);
    return QuickSelect(arr, k-pivot+begin-1, pivot+1, end);
}

int partition(int* arr, int first, int last) // chooses arr[last] as pivot
{
    int pivot = arr[last];
    int startIndex = first;
    for (int i = first; i < last; ++i)
    {
        if (arr[i] < pivot)
        {
            std::swap(arr[startIndex], arr[i]);
            ++startIndex;
        }
    }
    std::swap(arr[startIndex], arr[last]);
    return startIndex;
}

int kOrderStatistics(int* arr, int sz, int k)
{
    int left = 0, right = sz - 1;
    while (true)
    {
        int middle = partition(arr, left, right);
        if (middle < k)
        {
            left = middle + 1;
        } else if (middle == k) {
            return arr[k];
        } else {
            right = middle - 1;
        }
    }
    return -1;
}

int main(){
    constexpr int sz = 10;
    int arr[sz] = {-2, -3, 1, 2, 0, -1, 2, -3, -4, -1};
    for (int i = 0; i < sz; ++i)
    {
        std::cout << arr[i] << ' ';
    }
    std::cout << std::endl;
    for (int i = 0; i < sz; ++i)
    {
        std::cout << kOrderStatistics(arr, sz, i) << ' ';
    }
    std::cout << std::endl;
}
