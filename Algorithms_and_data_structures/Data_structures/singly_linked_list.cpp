#include <iostream>

struct Node{
    int value;
    Node* next;
    Node(int _value): value(_value), next(nullptr){};
};

struct Singly_Linked_List{
    Node* first;
    Node* last;
    Singly_Linked_List(): first(nullptr), last(nullptr){};
    bool IsEmpty(){
        return (first==nullptr);
    }
    void PushBack(int _value){
        Node* node = new Node(_value);
        if (IsEmpty()){
            first = node;
            last = node;
            return;
        }
        last->next = node;
        last = node;
    }
    void PopBack(){
        if (!IsEmpty()){
            Node* p = first;
            if (first == last){
                delete p;
                first = nullptr;
                last = nullptr;
                return;
            }
            while (p->next->next!=nullptr)
                p = p->next;
            delete p->next;
            p->next = nullptr;
            last = p;
            if (IsEmpty())
                first = nullptr;
        }
    }
    void Print(){
        Node* p = first;
        while (p){
            std::cout << p->value << ' ';
            p = p->next;
        }
        std::cout << '\n';
    }
    int Size(){
        if (IsEmpty())
            return 0;
        int result = 1;
        Node* p = first;
        while (p->next){
            p = p->next;
            result++;
        }
        return result;
    }
    void DeleteAt(int index){
        if (!IsEmpty()){
            Node* p = first;
            if (!index){
                if (Size()==1){
                    first = nullptr;
                    last = nullptr;
                    delete p;
                    return;
                }
                first = p->next;
                delete p;
                return;
            }
            for (int i=0; i<index-1; i++){
                p = p->next;
                if (!p)
                    return;
            }
            if (!p->next->next)
                last = p;
            Node* p_temp = p->next;
            p->next = p->next->next;
            delete p_temp;
        }
    }
    int operator [] (int num){
        if (!IsEmpty()){
            Node* p = first;
            for (int i=0; i<num; i++)
                p = p->next;
            return p->value;
        }
        return -1;
    }
    void Insert(int index, int value){
        if (!IsEmpty()){
            if (!index){
                Node* node = new Node(value);
                node->next = first;
                first = node;
                return;
            }
            if (index==Size()){
                PushBack(value);
                return;
            }
            Node* p = first;
            for (int i=0; i<index-1; i++){
                p = p->next;
                if (!p)
                    return;
            }
            Node* node = new Node(value);
            node->next = p->next;
            p->next = node;
        }
        if (!index)
            PushBack(value);
    }
};

int main(){
    Singly_Linked_List sll;
    for (int i=0; i<7; i++)
        sll.PushBack(2*i+5);
    sll.Insert(0, 1000);
    sll.Insert(1, 100);
    sll.Insert(10, 500);
    sll.Print();
    return 0;
}