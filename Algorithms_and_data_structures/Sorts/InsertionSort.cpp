#include <iostream>
#include <ctime>

void Swap(int& a, int& b){
    int t = a;
    a = b;
    b = t;
}

void Print(int* a, int size){
    for (int i=0; i<size; ++i){
        std::cout << *(a+i) << ' ';
    }
    std::cout << '\n';
}

int BinarySearch(int* a, int start, int end, int num){
    while (start<end){
        int mid = (start+end)/2;
        if (a[mid-1]<=num && a[mid]>=num)
            return mid;
        else if (a[mid]<num)
            start = mid+1;
        else
            end = mid-1;
    }
    return start;
}

void Insert(int* a, int i1, int i2){
    int t = a[i2];
    for (int i=i2-1; i>=i1; --i){
        a[i+1]=a[i];
    }
    a[i1] = t;
}

void InsertionSort(int* a, int size){
    int i=1;    
    while (i<size){
        if (a[i]<a[i-1])
            Insert(a, BinarySearch(a, 0, i, a[i]), i);
        i++;
    }
}

/*void InsertionSort(int* a, int size){
    for (int i=0; i<size-1; ++i){
        int j=i-1;
        while (j>=0 && a[j]>a[j+1]){
            Swap(a[j], a[j+1]);
            j--;
        }
    }
}*/

int main(){
    srand(time(NULL));
    int size = 1000;
    int a[size];
    for (int i=0; i<size; ++i)
        a[i] = rand()%size;
    //Print(a, size);
    InsertionSort(a, size);
    Print(a, size);
    return 0;
}