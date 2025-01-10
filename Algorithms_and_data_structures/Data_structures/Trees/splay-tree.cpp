#include <iostream>
#include <random>

template <typename T>
class SplayTree
{
    struct Node
    {
        T key;
        Node* parent;
        Node* left = nullptr;
        Node* right = nullptr;
        Node(T _key, Node* _parent = nullptr);
    };

    Node* root = nullptr;

    void print_rec(Node* node, int depth) const;
    void clear_rec(Node* node);
    Node* rotate_left(Node* x);
    Node* rotate_right(Node* x);
    void splay(Node* node);
    Node* merge(Node* _left, Node* right);

public:

    SplayTree();
    ~SplayTree();
    void print() const;
    void clear();
    void insert(T _key);
    Node* find(T _key) const;
    bool remove(T _key);
};

template <typename T>
SplayTree<T>::Node::Node(T _key, Node* _parent): key(_key), parent(_parent) {}

template <typename T>
void SplayTree<T>::print_rec(Node* node, int depth) const
{
    if (!node) return;
    print_rec(node->right, depth + 1);
    for (int i = 0; i < depth; ++i) std::cout << '\t';
    std::cout << node->key << ' ';
    if (!node->parent) std::cout << "null\n";
    else std::cout << node->parent->key << '\n';
    print_rec(node->left, depth + 1);
}

template <typename T>
void SplayTree<T>::clear_rec(Node* node)
{
    if (!node) return;
    clear_rec(node->left);
    clear_rec(node->right);
    delete node;
}

template <typename T>
typename SplayTree<T>::Node* SplayTree<T>::rotate_left(Node* x)
{
    Node* y = x->right;
    x->right = y->left;
    if (x->right) x->right->parent = x;
    y->parent = x->parent;
    if (y->parent)
    {
        if (y->key < y->parent->key) y->parent->left = y;
        else y->parent->right = y;
    } else root = y;
    x->parent = y;
    y->left = x;
    return y;
}

template <typename T>
typename SplayTree<T>::Node* SplayTree<T>::rotate_right(Node* x)
{
    Node* y = x->left;
    x->left = y->right;
    if (x->left) x->left->parent = x;
    y->parent = x->parent;
    if (y->parent)
    {
        if (y->key < y->parent->key) y->parent->left = y;
        else y->parent->right = y;
    } else root = y;
    x->parent = y;
    y->right = x;
    return y;
}

template <typename T>
void SplayTree<T>::splay(Node* node)
{
    while (root != node)
    {
        if (node->parent->left == node) node = rotate_right(node->parent);
        else node = rotate_left(node->parent);
    }
}

template <typename T>
typename SplayTree<T>::Node* SplayTree<T>::merge(Node* _left, Node* _right)
{
    if (!_left) return _right;
    if (!_right) return _left;
    Node* min = _right;
    while (min->left) min = min->left;
    min->left = _left;
    _left->parent = min;
    return _right;
}

template <typename T>
SplayTree<T>::SplayTree() {}

template <typename T>
SplayTree<T>::~SplayTree() { clear(); }

template <typename T>
void SplayTree<T>::print() const { print_rec(root, 0); }

template <typename T>
void SplayTree<T>::clear()
{
    clear_rec(root);
    root = nullptr;
}

template <typename T>
void SplayTree<T>::insert(T _key)
{
    if (!root)
    {
        Node* new_node = new Node(_key);
        root = new_node;
        return;
    }
    Node* tmp = root;
    while (_key != tmp->key && 
        ((tmp->left && _key < tmp->key) || (tmp->right && _key > tmp->key)))
    {
        tmp = _key < tmp->key ? tmp->left : tmp->right;
    }
    if (_key == tmp->key) return;
    Node* new_node = new Node(_key, tmp);
    if (_key < tmp->key) tmp->left = new_node;
    else tmp->right = new_node;
    splay(new_node);
}

template <typename T>
typename SplayTree<T>::Node* SplayTree<T>::find(T _key) const
{
    if (!root) return NULL;
    Node* tmp = root;
    while (_key != tmp->key && 
        ((tmp->left && _key < tmp->key) || (tmp->right && _key > tmp->key)))
    {
        tmp = _key < tmp->key ? tmp->left : tmp->right;
    }
    if (_key == tmp->key)
    {
        splay(tmp);
        return tmp;
    }
    return NULL;
}

template <typename T>
bool SplayTree<T>::remove(T _key)
{
    Node* tmp = find(_key);
    if (!tmp) return false;
    Node* left = tmp->left;
    Node* right = tmp->right;
    if (left) left->parent = nullptr;
    if (right) right->parent = nullptr;
    delete tmp;
    root = merge(left, right);
    return true;
}

int main()
{
    SplayTree<int> t;
    t.insert(1);
    t.~SplayTree();
}
