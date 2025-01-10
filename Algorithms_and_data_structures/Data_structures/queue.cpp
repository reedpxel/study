#include <iostream>

struct Node{
    int value;
    Node* prev;
    Node* next;
    Node(int _value): value(_value), prev(nullptr), next(nullptr){};
};

struct Queue{
    Node* first;
    Node* last;
    Queue(): first(nullptr), last(nullptr){};
    bool IsEmpty(){
        return !first;
    }
    void Push(int _value){
        Node* node = new Node(_value);
        if (!IsEmpty()){
            node->next = first;
            first->prev = node;
        }
        if (IsEmpty())
            last = node;
        first = node; 
    }
    void Pop(){
        if (!IsEmpty()){
            if (first!=last){
                last = last->prev;
                delete last->next;
                last->next = nullptr;
            }
            else{
                delete last;
                first = nullptr;
                last = nullptr;
            }
        }
    }
    void Print(){
        Node* temp = first;
        while (temp){
            std::cout << temp->value << ' ';
            temp = temp->next;
        }
        std::cout << '\n';
    }
    int Size(){
        Node* temp = first;
        int counter = 0;
        while (temp){
            counter++;
            temp = temp->next;
        }
        return counter;
    }
    int Peek(){
        return last->value;
    }
};

int main(){
    Queue q;
    for (int i=0; i<10; i++)
        q.Push(i);
    q.Print();
    for (int i=0; i<9; i++)
    q.Pop();
    q.Print();
    q.Pop();
    q.Pop();
    q.Pop();
    q.Print();
    std::cout << q.Size() << '\n';
    for (int i=0; i<5; i++)
        q.Push(4*i);
    std::cout << q.Size() << '\n';
    q.Print();
    std::cout << q.Peek() << '\n';
    q.Pop();    
    std::cout << q.Peek();
    return 0;
}