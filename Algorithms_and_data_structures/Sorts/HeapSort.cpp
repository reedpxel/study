#include <iostream>
#include <cmath>
#include <ctime>
#define min(a, b) ((a<=b)? a:b)

class BinaryHeap{
    private:
        int Parent(int i){return (i-1)/2;}
        int Left_Child(int i){return 2*i+1;}
        int Right_Child(int i){return 2*i+2;}
        int Generation(int i){return (int)floor(log2(i+1));}
        void Swap(int& a, int& b){
            int temp=a;
            a=b;
            b=temp;
        }
    public:
        int* array;
        int capacity;
        int size;
        BinaryHeap(int _capacity){
            array = new int[_capacity];
            capacity = _capacity;
            size = 0;
        }
        BinaryHeap(int* _array, int _size): array(_array), capacity(_size), size(_size) {};
        /*~BinaryHeap(){
            delete[] array;
        }*/
        void Heapify(){
            for (int i=size-1; i>=0; --i){
                int i_=i;
                while (array[i_]<array[Parent(i_)] && i_>=0 && Parent(i_)>=0){
                    Swap(array[i_], array[Parent(i_)]);
                    i_ = Parent(i_);
                }
            }
        }
        void Insert(int value){
            int i = size;
            array[i] = value;
            while (i>0 && array[i]<array[Parent(i)]){ //SiftUp
                Swap(array[i], array[Parent(i)]);
                i = Parent(i);
            }
            size++;
        }
        void Print(){
            for (int i=0; i<size; i++){
                std::cout << array[i] << ' ';
                if (Generation(i)!=Generation(i+1))
                    std::cout << '\n';
            }
            std::cout << '\n';
        }
        int ExtractMin(){
            int temp = array[0];
            Swap(array[0], array[size-1]);
            --size;
            int i=0;
            while (i<=size && Left_Child(i)<size &&
                 (array[i]>array[Left_Child(i)] || (array[i]>array[Right_Child(i)])
                  && Right_Child(i)<size)){ //SiftDown
                int i_;
                if (Right_Child(i)<size)
                    i_ = (array[Left_Child(i)]>=array[Right_Child(i)])? Right_Child(i) : Left_Child(i);
                else
                    i_ = Left_Child(i);
                Swap(array[i], array[i_]);
                i = i_;
            }
            
            return temp;
        }
};

void HeapSort(int* array, int size){
    int array_[size];
    for (int i=0; i<size; ++i)
        array_[i] = array[i];
    BinaryHeap bh = BinaryHeap(array_, size);
    bh.Heapify();
    for (int i=0; i<size; ++i)
        array[i] = bh.ExtractMin();
}

int main(){
    srand(time(NULL));
    int size = 1000;
    int array[size];
    for (int i=size-1; i>=0; --i)
        array[i] = rand()%100;
    for (int i=0; i<size; ++i)
        std::cout << array[i] << ' ';
    std::cout << '\n';
    double start;
    double duration;
    start = std::clock();
    HeapSort(array, size);
    duration = (std::clock()-start)/(double)CLOCKS_PER_SEC;
    for (int i=0; i<size; ++i)
        std::cout << array[i] << ' ';
    std::cout << '\n' << duration << " ms";
    return 0;
}