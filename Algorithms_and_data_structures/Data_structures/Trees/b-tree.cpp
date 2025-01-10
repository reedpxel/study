#include <iostream>
#include <ctime>

template <typename KeyType, int order>
class BTree
{
public:

    struct Node
    {
        KeyType* keys;
        Node** children;
        Node* parent;
        unsigned long long size;
        bool isLeaf;

        Node();
        ~Node();
        void insert_not_full(KeyType _key);
        void remove_not_min_size_leaf(KeyType _key);
    };
    
    Node* root;

    void print_rec(Node* node, int depth);
    Node* find_rec(Node* node, KeyType _key);
    void clear_rec(Node* node);
    Node* divide(Node* node);
    Node* merge_leafs(Node* left, Node* right);
    void return_order(Node* node);

public:
    
    BTree();
    ~BTree();
    void clear();
    void insert(KeyType _key);
    void print();
    Node* find(KeyType _key);
    bool remove(KeyType _key);
};

template <typename KeyType, int order>
BTree<KeyType, order>::Node::Node(): parent(nullptr),
                                     keys(new KeyType[2 * order - 1]), 
                                     children(new Node*[2 * order]()),
                                     size(1), 
                                     isLeaf(true) {}

template <typename KeyType, int order>
BTree<KeyType, order>::Node::~Node()
{
    delete[] keys;
    delete[] children;
}

template <typename KeyType, int order>
void BTree<KeyType, order>::Node::insert_not_full(KeyType _key) 
{
    ++size;
    if (_key < keys[0])
    {
        for (int i = size - 1; i > 0; --i)
        {
            keys[i] = keys[i - 1];
        }
        keys[0] = _key;
        return;
    }
    for (int i = 0; i < size - 1; ++i)
    {
        if (_key == keys[i])
        {
            --size;
            return;
        }
        if (_key < keys[i])
        {
            for (int j = size - 1; j > i; --j)
            {
                keys[j] = keys[j - 1];
            }
            keys[i] = _key;
            return;
        }
    }
    keys[size - 1] = _key;
}

template <typename KeyType, int order>
typename BTree<KeyType, order>::Node* BTree<KeyType, order>::find(KeyType _key) { return find_rec(root, _key); }

template <typename KeyType, int order>
typename BTree<KeyType, order>::Node* BTree<KeyType, order>::find_rec(Node* node, KeyType _key)
{
    if (_key < node->keys[0])
    {
        if (node->isLeaf) return NULL;
        return find_rec(node->children[0], _key);
    }
    for (int i = 0; i < node->size; ++i)
    {
        if (node->keys[i] == _key) return node;
        if (node->keys[i] > _key) return find_rec(node->children[i], _key);
    }
    if (node->isLeaf) return NULL;
    return find_rec(node->children[node->size], _key);
}

template <typename KeyType, int order>
BTree<KeyType, order>::BTree(): root(nullptr) {}

template <typename KeyType, int order>
BTree<KeyType, order>::~BTree() { clear(); }

template <typename KeyType, int order>
void BTree<KeyType, order>::clear() { clear_rec(root); }

template <typename KeyType, int order>
void BTree<KeyType, order>::clear_rec(Node* node)
{
    if (!node) return;
    if (node->children)
    {
        for (int i = 0; i < node->size + 1; ++i)
        {
            clear_rec(node->children[i]);
        }
        delete[] node->children;
    }
    delete[] node->keys;
}

template <typename KeyType, int order>
void BTree<KeyType, order>::insert(KeyType _key)
{
    if (!root)
    {
        root = new Node();
        root->keys[0] = _key;
        return;
    }
    Node* tmp = root;
    while (!tmp->isLeaf)
    {
        if (_key > tmp->keys[tmp->size - 1]) tmp = tmp->children[tmp->size];
        else
        {
            for (int i = 0; i < tmp->size; ++i)
            {
                if (tmp->keys[i] == _key) return;
                if (_key < tmp->keys[i])
                {
                    tmp = tmp->children[i];
                    break;
                }
            }
        }
    }
    if (tmp->size == 2 * order - 1)
    {
        Node* tmp_root = divide(tmp);
        if (_key < tmp_root->keys[0]) tmp = tmp_root->children[0];
        else
        {
            for (int i = 1; i < tmp_root->size; ++i)
            {
                if (_key < tmp_root->keys[i])
                {
                    tmp = tmp_root->children[i];
                    break;
                }
            }
            if (_key > tmp_root->keys[tmp_root->size - 1])
            {
                tmp = tmp_root->children[tmp_root->size];
            }
        }
    }
    tmp->insert_not_full(_key);
}

template <typename KeyType, int order>
void BTree<KeyType, order>::print(){ print_rec(root, 0); }

template <typename KeyType, int order>
void BTree<KeyType, order>::print_rec(Node* node, int depth)
{
    if (!node) return;
    for (int i = node->size; i >= 0; --i)
    {
        if (node->children[i]) print_rec(node->children[i], depth + 1);
    }
    for (int i = 0; i < depth; ++i) std::cout << "\t\t";
    for (int i = 0; i < node->size; ++i) std::cout << node->keys[i] << ' ';
    std::cout << "par: ";
    if (!node->parent) std::cout << "nullptr ";
    else std::cout << node->parent->keys[0] << ' ';
    std::cout << node->isLeaf << ' ' << node->size << '\n';
}

template <typename KeyType, int order>
typename BTree<KeyType, order>::Node* BTree<KeyType, order>::divide(Node* node) // the node has 2t-1 elements and an element must be inserted in it
{
    Node* left = new Node();
    Node* right = new Node();
    left->size = order - 1;
    right->size = order - 1;
    if (node == root)
    {
        Node* new_root = new Node();
        left->parent = new_root;
        right->parent = new_root;
        new_root->keys[0] = node->keys[order - 1];
        new_root->children[0] = left;
        new_root->children[1] = right;
        for (int i = 0; i < order; ++i)
        {
            if (i != order - 1) left->keys[i] = root->keys[i]; 
            if (root->children[i])
            {
                left->children[i] = root->children[i];
                left->children[i]->parent = left;
            }
        }
        for (int i = order; i < 2 * order; ++i)
        {
            if (i != 2 * order - 1) right->keys[i - order] = root->keys[i];
            if (root->children[i - order])
            {
                right->children[i - order] = root->children[i];
                right->children[i - order]->parent = right;
            }
        }
        if (!root->isLeaf)
        {
            left->isLeaf = false;
            right->isLeaf = false;
        }
        new_root->isLeaf = false;
        root->~Node();
        root = new_root;
        return root;
    } else {
        Node* par = node->parent;
        if (par->size == 2 * order - 1)
        {
            divide(par);
            par = node->parent;
        }
        ++par->size;
        int i = 0;
        while (par->children[i] != node) ++i;
        for (int j = par->size - 1; j >= i; --j)
        {
            if (j != par->size - 1) par->keys[j + 1] = par->keys[j];
            par->children[j + 1] = par->children[j];
        } 
        par->keys[i] = node->keys[order - 1];
        par->children[i] = left;
        par->children[i + 1] = right;
        left->parent = par;
        right->parent = par;
        for (int i = 0; i < order; ++i)
        {
            if (i != order - 1) left->keys[i] = node->keys[i]; 
            if (node->children[i])
            {
                left->children[i] = node->children[i];
                left->children[i]->parent = left;
            }
        }
        for (int i = order; i < 2 * order; ++i)
        {
            if (i != 2 * order - 1) right->keys[i - order] = node->keys[i];
            if (node->children[i - order])
            {
                right->children[i - order] = node->children[i];
                right->children[i - order]->parent = right;
            }
        }
        if (left->children[0]) left->isLeaf = false;
        if (right->children[0]) right->isLeaf = false;
        node->~Node();
        return par;
    }
}

template <typename KeyType, int order>
void BTree<KeyType, order>::Node::remove_not_min_size_leaf(KeyType _key)
{
    for (int i = 0; i < size; ++i)
    {
        if (_key == keys[i])
        {
            for (int j = i; j < size; ++j)
            {
                keys[j] = keys[j + 1];
            }
            --size;
            return;
        }
    }
}

template <typename KeyType, int order>
typename BTree<KeyType, order>::Node* BTree<KeyType, order>::merge_leafs(Node* left, Node* right)
{
    int left_size = left->size;
    int right_size = right->size;
    for (int i = 0; i < right_size; ++i)
    {
        left->keys[i + left_size] = right->keys[i];
    }
    left->size += right_size;
    right->~Node();
    return left;
}

template <typename KeyType, int order>
bool BTree<KeyType, order>::remove(KeyType _key)
{
    Node* node = find(_key);
    if (!node) return false;
    if (node == root && node->size == 1)
    {
        root = nullptr;
        node->~Node();
        return true;
    }
    if (node->isLeaf)
    {
        node->remove_not_min_size_leaf(_key);
        return_order(node);
    } else {
        int i = 0;
        for (; i < node->size; ++i)
        {
            if (_key == node->keys[i]) break;
        }
        Node* to_swap = node->children[i + 1];
        while (to_swap->children[0]) to_swap = to_swap->children[0];
        KeyType tmp = to_swap->keys[0];
        node->keys[i] = tmp;
        to_swap->remove_not_min_size_leaf(tmp);
        return_order(to_swap);
    }
    return true;
}

template <typename KeyType, int order>
void BTree<KeyType, order>::return_order(Node* node)
{
    if (node->size >= order - 1) return;
    if (node == root)
    {
        if (!node->size)
        {
            Node* tmp = root;
            root = tmp->children[0];
            tmp->~Node();
            root->parent = nullptr;
            if (!root->children[0]) root->isLeaf = true;
        }
        return;
    }
    Node* par = node->parent;
    int i = 0;
    for (; i < par->size; ++i)
    {
        if (par->children[i] == node) break;
    }
    if (node->isLeaf)
    {
        if (i && par->children[i - 1]->size >= order)
        {
            node->insert_not_full(par->keys[i - 1]);
            Node* child = par->children[i - 1];
            par->keys[i - 1] = child->keys[child->size - 1];
            --child->size;
            Node* tmp = find(par->keys[0]);
            if (tmp)
            {
                for (int k = 0; k < tmp->size + 1; ++k)
                {
                    tmp->children[k]->parent = tmp;
                }
            }
            return;
        } else if (i < par->size && par->children[i + 1]->size >= order) {
            node->insert_not_full(par->keys[i]);
            Node* child = par->children[i + 1];
            par->keys[i] = child->keys[0];
            for (int j = 0; j < child->size - 1; ++j)
            {
                child->keys[j] = child->keys[j + 1];
            }
            --child->size;
            Node* tmp = find(par->keys[0]);
        if (tmp)
        {
            for (int k = 0; k < tmp->size + 1; ++k)
            {
                tmp->children[k]->parent = tmp;
            }
        }
            return;
        }
    }
    int old_size = node->size;
    Node* child;
    if (i)
    {
        child = par->children[i - 1];
        if (child->size == 2 * order - 1)
        {
            ++node->size;
            node->keys[node->size - 1] = par->keys[i];
            for (int j = 1; j < node->size; ++j)
            {
                node->children[j] = node->children[j - 1];
            }
            node->children[0] = child->children[child->size];
            par->keys[i] = child->keys[0];
            --child->size;
            return_order(par);
            return;
            
        }
        node->insert_not_full(par->keys[i - 1]);
        for (int j = child->size; j > 0; --j)
        {
            node->insert_not_full(child->keys[j - 1]);
        }
        for (int j = i; j < par->size + 1; ++j)
        {
            if (j != par->size) par->keys[j - 1] = par->keys[j];
            par->children[j - 1] = par->children[j];
        }
    } else {
        child = par->children[1];
        if (child->size == 2 * order - 1)
        {
            ++node->size;
            node->keys[node->size - 1] = par->keys[i];
            node->children[node->size] = child->children[0];
            par->keys[i] = child->keys[0];
            for (int j = 0; j < child->size; ++j)
            {
                if (j != child->size - 1) child->keys[i] = child->keys[i + 1];
                child->children[i] = child->children[i + 1];
            }
            --child->size;
            return_order(par);
            return;
        }
        node->size += child->size + 1;
        node->keys[old_size] = par->keys[0];
        for (int j = old_size + 1; j < node->size + 1; ++j)
        {
            node->keys[j] = child->keys[j - old_size - 1];
        }
        for (int j = 0; j < child->size + 1; ++j)
        {
            node->children[j + old_size + 1] = child->children[j];
        }
        for (int j = 1; j < par->size + 1; ++j)
        {
            par->keys[j - 1] = par->keys[j];
            par->children[j] = par->children[j + 1];
        }
    }
    for (int j = 0; j < child->size + 1; ++j)
    {
        node->children[j + old_size + 1] = child->children[j];
    }
    --par->size;
    Node* tmp = find(par->keys[0]);
    if (tmp)
    {
        for (int k = 0; k < tmp->size + 1; ++k)
        {
            tmp->children[k]->parent = tmp;
        }
    }
    child->~Node();
    return_order(par);
}

int main()
{
    BTree<int, 3> t;
    for (int i = 0; i < 100; ++i) t.insert(i);
    //t.print();
    for (int i = 0; i < 72; ++i) t.remove(i);
    t.print();
    t.remove(72);
    t.print();
}
