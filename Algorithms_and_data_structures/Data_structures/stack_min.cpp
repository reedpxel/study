#include <iostream>
#define _min(a, b) ((a<=b)? a:b)

struct NodeMin{
    int value;
    NodeMin* next;
    int min;
    NodeMin(int _value): value(_value), next(nullptr), min(_value){};
};

class Stack{
public:
    NodeMin* top;
    int size;
    Stack(): top(nullptr), size(0){};
    bool IsEmpty(){
        return !size;
    }
    void Push(int _value){
        NodeMin* node = new NodeMin(_value);
        if (!IsEmpty()){
            node->next = top;
            node->min = _min(node->value, node->next->min);
        }
        top = node;
        size++;
    }
    void Pop(){
        if (!IsEmpty()){
            NodeMin* temp = top;
            top = top->next;
            size--;
            delete temp;
        }
    }
    void Print(){
        NodeMin* temp = top;
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
    int Min(){
        return top->min;
    }
};

int main(){
    Stack s;
    s.Push(11);
    for (int i=0; i<10; i++)
        s.Push(i+10);
    s.Print();
    std::cout << s.Top() << '\n' << s.Size() << '\n' << s.Min();
    return 0;
}
