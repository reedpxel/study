#include <iostream>
#include <vector>

// hash tables are implemented in STL under the names unordered_map(stores keys and values) and unordered_set (stores only keys)

class HashTable
{
public:
    struct Node
    {
        std::string key;
        std::string value;
        Node* prev;
        Node* next;
        Node(std::string _key, std::string _value);
    };
    
    Node** arr;
    size_t m;
    Node** endings;
    int hash(std::string _key) const;

public:
    HashTable();
    HashTable(size_t buffer_size);
    ~HashTable();
    void insert(std::string _key, std::string _value);
    void erase(std::string _key);
    Node* find(std::string _key) const;
    void print() const;
    bool contains(std::string _key) const;
};

HashTable::Node::Node(std::string _key, std::string _value) : key(_key), value(_value), prev(nullptr), next(nullptr) {}

HashTable::HashTable(): arr(new Node*[11]()), m(11), endings(new Node*[11]) {}

HashTable::HashTable(size_t buffer_size) : arr(new Node*[buffer_size]()), m(buffer_size), endings(new Node*[buffer_size]()) {}

HashTable::~HashTable()
{
    for (int i = 0; i < m; ++i)
    {
        if (arr[i])
        {
            if (arr[i]->next)
            {
                Node* p1 = arr[i];
                Node* p2 = p1->next;
                while (p2)
                {
                    delete p1;
                    p1 = p2;
                    p2 = p1->next;
                }
            } else {
                delete arr[i];
            }
        }
    }
    delete[] arr;
    delete[] endings;
}

int HashTable::hash(std::string _key) const
{
    int res = 0;
    for (int i = 0; i < _key.size(); ++i) res += _key[i] + 127;
    return res % m;    
}

void HashTable::insert(std::string _key, std::string _value)
{
    int _hash = hash(_key);
    typename HashTable::Node* new_node = new Node(_key, _value);
    if (!arr[_hash])
    {
        arr[_hash] = new_node;
    } else {
        new_node->prev = endings[_hash];
        endings[_hash]->next = new_node;
    }
    endings[_hash] = new_node;
}

void HashTable::print() const
{
    for (int i = 0; i < m; ++i)
    {
        std::cout << i << ") ";
        if (!arr[i]) std::cout << "0\n";
        else
        {
            Node* p = arr[i];
            while (p)
            {
                std::cout << p->key << " * " << p->value << " | ";
                p = p->next;
            }
            std::cout << '\n';
        }
    }
}

typename HashTable::Node* HashTable::find(std::string _key) const
{
    int _hash = hash(_key);
    if (!arr[_hash]) return nullptr;
    Node* p = arr[_hash];
    while (p)
    {
        if (p->key == _key) return p;
        p = p->next;
    }
    return nullptr;
}

bool HashTable::contains(std::string _key) const { return find(_key) ? true : false; }

void HashTable::erase(std::string _key)
{
    int _hash = hash(_key);
    if (!arr[_hash]) return;
    Node* p = arr[_hash];
    while (p)
    {
        if (p->key == _key) break;
        p = p->next;
    }
    if (!p) return;
    if (arr[_hash] == p)
    {
        if (endings[_hash] == p)
        {
            arr[_hash] = nullptr;
            endings[_hash] = nullptr;
            delete p;
            return;
        }
        arr[_hash] = p->next;
        p->next->prev = nullptr;
        delete p;
        return;
    }
    if (endings[_hash] == p)
    {
        endings[_hash] = p->prev;
        p->prev->next = nullptr;
        delete p;
        return;
    }
    p->prev->next = p->next;
    delete p;
}

int main()
{
    HashTable t;
    t.insert("nickname1", "qwer");
    t.insert("qqq", "asdfvxvzx");
    t.insert("asdff", "proladf");
    t.erase("qqq");
    t.erase("asdf");
    t.print();
}

