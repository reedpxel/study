#include <iostream>

struct Node{
    int value;
    Node* next;
    Node(int _value): value(_value), next(nullptr){};
};

class Stack{
public:
    Node* top;
    int size;
    Stack(): top(nullptr), size(0){};
    bool IsEmpty(){
        return !size;
    }
    void Push(int _value){
        Node* node = new Node(_value);
        if (!IsEmpty())
            node->next = top;
        top = node;
        size++;
    }
    void Pop(){
        if (!IsEmpty()){
            Node* temp = top;
            top = top->next;
            size--;
            delete temp;
        }
    }
    void Print(){
        Node* temp = top;
        while (temp){
            std::cout << temp->value << " ";
            temp = temp->next;
        }
        std::cout << '\n';
    }
    int Top(){
        return top->value;
    }
    int Size(){
        return size;
    }
};

int main(){
    Stack s;
    for (int i=0; i<10; i++)
        s.Push(i);
    s.Print();
    std::cout << s.Top() << '\n' << s.Size() << '\n';
    return 0;
}
