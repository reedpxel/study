#include <iostream>

struct Node{
    int value;
    Node* prev;
    Node* next;
    Node(int _value): value(_value), prev(nullptr), next(nullptr){};
};

struct Double_Linked_List{
    Node* first;
    Node* last;
    Double_Linked_List(): first(nullptr), last(nullptr){};
    bool IsEmpty(){
        return first==nullptr;
    }
    void PushBack(int _value){
        Node* node = new Node(_value);
        if (IsEmpty()){
            first = node;
            last = node;
            return;
        }
        last->next = node;
        node->prev = last;
        last = node;
    }
    void PushFront(int _value){
        Node* node = new Node(_value);
            if (IsEmpty()){
            first = node;
            last = node;
            return;
        }
        first->prev = node;
        node->next = first;
        first = node;
    }
    void PopBack(){
        if (!IsEmpty()){
            Node* p = last;
            last = p->prev;
            p->prev->next = nullptr;
            if (!last)
                first = nullptr;
            delete p;
        }
    }
    void PopFront(){
        if (!IsEmpty()){
            Node* p = first;
            first = p->next;
            p->next->prev = nullptr;
            if (!first)
                last = nullptr;
            delete p;
        }
    }
    void Print(){
        Node* p = first;
        while (p){
            std::cout << p->value << " ";
            p = p->next;
        }
        std::cout << '\n';
    }
    int Size(){
        Node* p = first;
        int result = 0;
        while (p){
            p = p->next;
            result++;
        }
        return result;
    }
    int operator [] (int index){
        if (!IsEmpty()){
            Node* p = first;
            for (int i=0; i<index; i++)
                p = p->next;
            return p->value;
        }
        return -1;
    }
    void DeleteAt(int index){
        if (IsEmpty()){
            return;
        }
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
    void Insert(int index, int value){
        if (!IsEmpty()){
            if (!index){
                PushFront(value);
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
            node->prev = p;
            node->next = p->next;
            p->next = node;
            node->next->prev = node;
        }
        if (!index)
            PushBack(value);
    }
};

int main(){
    Double_Linked_List dll;
    for (int i=0; i<10; i++)
        dll.Insert(0, 10*i);
    dll.DeleteAt(5);
    std::cout << dll.Size() << '\n';
    dll.Print();
}