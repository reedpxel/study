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

int main(){
    int size = 10;
    srand(time(NULL));
    int arr[size] = {6, 3, 8, 6, 2, 1, 5, -10, 3, 2};
    for (int i=0; i<size; ++i)
        std::cout << arr[i] << ' ';
    std::cout << '\n';
    for (int i=0; i<10; ++i)
        std::cout << QuickSelect(arr, i, 0, 9) << ' ';
    return 0;
}