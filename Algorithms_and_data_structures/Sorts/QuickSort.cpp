#include <iostream>
#include <cmath>
#include <ctime>

void Swap(int& a, int& b){
    int t = a;
    a = b;
    b = t;
}

void Print(int array[], int size){
    for (int i=0; i<size; ++i)
        std::cout << array[i] << " ";
    std::cout << '\n';
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

int PartitionStart(int* array, int start, int end){
    int pivot = array[start];
    int start_index = start;
    for (int i=start; i<=end; ++i){
        if (array[i]<pivot){
            Swap(array[start_index+1], array[i]);
            Swap(array[start_index], array[start_index+1]);
            ++start_index;
        }
    }
    return start_index;
}

int PartitionHoare(int* array, int start, int end){
    int pivot = array[start];
    int i = start-1, j=end+1;
    while (true){
        do {++i;}
        while (array[i]<pivot);
        do {--j;}
        while (array[j]>pivot);
        if (i>=j)
            return j;
        Swap(array[i], array[j]);
    }
}

// чтобы сделать Partition от рандомного эл-та массива, надо Swap(rand(), start)
// и сделать PartitionStart 

int RandomPartition(int* arr, int start, int end){
    srand(time(NULL));
    int pivot_index = rand()%(end-start)+start;
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

void QuickSort(int* array, int start, int end){
    if (start>=end)
        return;
    int pivot = PartitionStart(array, start, end);
    QuickSort(array, start, pivot-1);
    QuickSort(array, pivot+1, end);
}

void QuickSortHoare(int* array, int start, int end){
    if (start<end){
        int pivot = PartitionHoare(array, start, end);
        QuickSortHoare(array, start, pivot);
        QuickSortHoare(array, pivot+1, end);
    }
}

int main(){
    int array[10];
    srand(time(NULL));
    int size = sizeof(array)/sizeof(array[0]);
    for (int i=0; i<size; ++i)
        array[i] = rand()%size;
    Print(array, size);
    QuickSort(array, 0, size-1);
    std::cout << "********************\n";
    Print(array, size);
    return 0;
}
