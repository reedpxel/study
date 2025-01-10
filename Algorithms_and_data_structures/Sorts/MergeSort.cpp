#include <iostream>
#include <vector>
#include <ctime>

void Merge(int v[], const int begin, const int mid, const int end){
    int left[mid-begin+1], right[end-mid];
    for (int i=0; i<mid-begin+1; ++i) left[i] = v[begin+i];
    for (int i=0; i<end-mid; ++i) right[i] = v[mid+1+i];
    int i=0, j=0;
    while (i<mid-begin+1 && j<end-mid){
        if (left[i]<=right[j]){
            v[begin+i+j] = left[i];
            i++;
        }
        else {
            v[begin+i+j] = right[j];
            j++;
        }
    }
    for (int i=0; i<20; ++i)
    if (i>=mid-begin+1){
        while (j<end-mid){
            v[begin+i+j] = right[j];
            j++;
        }
    }
    if (j>=end-mid){
        while (i<mid-begin+1){
            v[begin+i+j] = left[i];
            i++;
        }
    }
}

void MergeSort(int v[], int begin, int end){
    if (begin>=end) return;
    int mid = begin + (end-begin)/2;
    MergeSort(v, begin, mid);
    MergeSort(v, mid+1, end);
    Merge(v, begin, mid, end);
}

bool ArrayIsSorted(int v[], int size){
    for (int i=0; i<size-1; ++i)
        if (v[i]>v[i+1])
            return false;
    return true;
}

int main(){
    double start, duration;
    start = std::clock();
    for (int i=0; i<1000; ++i){
        int v[20];
        srand(time(NULL));
        for (int i=0; i<20; ++i)
            v[i] = rand();
        MergeSort(v, 0, 19);
        if (!ArrayIsSorted(v, 20)){
            std::cout << '0';
            return 1;
        }
    }
    std::cout << '1' << '\n';
    duration = (std::clock()-start)/(double)CLOCKS_PER_SEC;
    std::cout << duration << " ms";
    return 0;
}