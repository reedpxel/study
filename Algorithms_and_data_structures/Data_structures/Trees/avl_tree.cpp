#include <iostream>
#include <queue>

template <typename T>
class AVLTree
{
public:
    struct Node
    {
        T key;
        unsigned height;
        Node* left;
        Node* right;
        Node* parent;
        size_t leftSize;
        size_t rightSize;

        Node(T key);
        unsigned _height() const;
        void fix();
        int bfactor() const;
    };

    Node* root;

    void rotate_left(Node* node);
    void rotate_right(Node* node);
    void balance(Node* node);
    void clear_rec(Node* node);
    void print_rec(Node* node, int depth) const;

public:
    AVLTree();
    ~AVLTree();
    void insert(T _key);
    bool remove(T _key);
    bool find(T _key) const;
    Node* find_ptr(T _key) const;
    void print() const;
    T k_stat(unsigned k) const;
};

template <typename T>
AVLTree<T>::Node::Node(T key): key(key), 
                               height(1), 
                               left(nullptr), 
                               right(nullptr), 
                               parent(nullptr), 
                               leftSize(0),
                               rightSize(0) {}

template <typename T>
unsigned AVLTree<T>::Node::_height() const { return this ? this->height : 0; }

template <typename T>
void AVLTree<T>::Node::fix()
{
    unsigned left_height = left->_height();
    unsigned right_height = right->_height();
    height = (left_height < right_height ? right_height : left_height) + 1;
    leftSize = left ? left->leftSize + left->rightSize + 1 : 0;
    rightSize = right ? right->leftSize + right->rightSize + 1 : 0;
}

template <typename T>
int AVLTree<T>::Node::bfactor() const { return right->_height() - left->_height(); }

template <typename T>
AVLTree<T>::AVLTree(): root(nullptr) {}

template <typename T>
AVLTree<T>::~AVLTree() { clear_rec(root); }

template <typename T>
void AVLTree<T>::rotate_left(Node* node)
{
    Node* tmp = node->right;
    tmp->parent = node->parent;
    if (tmp->parent)
    {
        if (node->key < tmp->parent->key) tmp->parent->left = tmp;
        else tmp->parent->right = tmp;
    } else root = tmp;
    node->right = tmp->left;
    if (node->right) node->right->parent = node;
    tmp->left = node;
    node->parent = tmp;
    node->fix();
    tmp->fix();
}

template <typename T>
void AVLTree<T>::rotate_right(Node* node)
{
    Node* tmp = node->left;
    tmp->parent = node->parent;
    if (tmp->parent)
    {
        if (node->key < node->parent->key) tmp->parent->left = tmp;
        else tmp->parent->right = tmp;
    } else root = tmp;
    node->left = tmp->right;
    if (node->left) node->left->parent = node;
    tmp->right = node;
    node->parent = tmp;
    node->fix();
    tmp->fix();
}

template <typename T>
void AVLTree<T>::balance(Node* node)
{
    if (!node) return;
    node->fix();
    if (node->bfactor() == 2)
    {
        if (node->right->bfactor() == -1) rotate_right(node->right);
        rotate_left(node);
    } else if (node->bfactor() == -2) {
        if (node->left->bfactor() == 1) rotate_left(node->left);
        rotate_right(node);
    }
}

template <typename T>
void AVLTree<T>::insert(T _key)
{
    Node* new_node = new Node(_key);
    if (!root)
    {
        root = new_node;
        return;
    }
    Node* tmp = root;
    while (tmp && 
          ((_key < tmp->key && tmp->left) || 
          (_key > tmp->key && tmp->right)))
    {
        tmp = _key < tmp->key ? tmp->left : tmp->right;
    }
    if (_key < tmp->key)
    {
        tmp->left = new_node;
    } else {
        tmp->right = new_node;
    }
    new_node->parent = tmp;
    while (tmp)
    {
        balance(tmp);
        tmp = tmp->parent;
    }
}

template <typename T>
bool AVLTree<T>::remove(T _key)
{
    Node* node = root;
    if (!node) return false;
    while (_key != node->key && ((_key < node->key && node->left) ||
                                 (_key > node->key && node->right)))
    {
        node = _key < node->key ? node->left : node->right;
    }
    if (_key != node->key) return false;
    Node* tmp_balance = node;
    if (!node->left)
    {
        if (!node->right)
        { // у удаляемого узла нет детей
            if (node == root) root = nullptr;
            if (node->parent)
            {
                if (node->key < node->parent->key) node->parent->left = nullptr;
                else node->parent->right = nullptr;
            }
        } else { // у узла есть только правый ребенок
            tmp_balance = node->right;
            if (node == root) root = node->right;
            if (node->parent)
            {
                if (node->key < node->parent->key) node->parent->left = node->right;
                else node->parent->right = node->right;
            }
            node->right->parent = node->parent;
        }
    } else {
        if (!node->right)
        { // у узла есть только левый ребенок
            tmp_balance = node->left;
            if (node == root) root = node->left;
            if (node->parent)
            {
                if (node->key < node->parent->key) node->parent->left = node->left;
                else node->parent->right = node->left;
            }
            node->left->parent = node->parent;
        } else { // у узла есть оба ребенка
            Node* tmp = node->left;
            while (tmp->right) tmp = tmp->right;
            if (root == node) root = tmp;
            tmp_balance = tmp->parent;
            if (tmp->left)
            {
                if (node != tmp_balance) tmp_balance->right = tmp->left;
            } else {
                if (node != tmp_balance) tmp_balance->right = nullptr;
            }
            tmp->parent = node->parent;
            if (node->parent)
            {
                if (node->key < node->parent->key) node->parent->left = tmp;
                else node->parent->right = tmp;
            }
            if (node != tmp_balance)
            {
                tmp->left = node->left;
                tmp->left->parent = tmp;
            }
            tmp->right = node->right;
            tmp->right->parent = tmp;
        }
    }
    while (tmp_balance)
    {
        balance(tmp_balance);
        tmp_balance = tmp_balance->parent;
    }
    delete node;
    return true;
}

template <typename T>
typename AVLTree<T>::Node* AVLTree<T>::find_ptr(T _key) const
{
    Node* tmp = root;
    while (_key != tmp->key && 
          ((_key < tmp->key && tmp->left) || (_key > tmp->key && tmp->right)))
    {
        tmp = _key < tmp->key ? tmp->left : tmp->right;
    }
    if (!tmp) return NULL;
    return tmp;
}

template <typename T>
bool AVLTree<T>::find(T _key) const
{
    Node* tmp = root;
    while (_key != tmp->key && tmp)
    {
        tmp = _key < tmp->key ? tmp->left : tmp->right;
    }
    if (!tmp) return false;
    return true;
}

template <typename T>
void AVLTree<T>::print() const { print_rec(root, 0); }

template <typename T>
void AVLTree<T>::print_rec(Node* node, int depth) const
{
    if (!node) return;
    print_rec(node->right, depth + 1);
    for (int i = 0; i < depth; ++i) std::cout << '\t';
    //std::cout << node->key << '\n';
    std::cout << node->key << ' ';
    if (node->parent) std::cout << node->parent->key << ' ';
    else std::cout << "nullptr ";
    if (node->left) std::cout << node->left->key << ' ';
    else std::cout << "nullptr ";
    if (node->right) std::cout << node->right->key << ' ';
    else std::cout << "nullptr ";
    std::cout << node->height << '\n';
    print_rec(node->left, depth + 1);
}

template <typename T>
void AVLTree<T>::clear_rec(Node* node)
{
    if (!node) return;
    clear_rec(node->left);
    clear_rec(node->right);
    delete node;   
}

template <typename T>
T AVLTree<T>::k_stat(unsigned k) const
{
    Node* node = root;
    while (k != node->leftSize)
    {
        if (node->leftSize < k)
        {
            k -= node->leftSize + 1;
            node = node->right;
        } else {
            node = node->left;
        }
    }
    return node->key;
}

int main()
{
    AVLTree<int> t;
    for (int i = 0; i < 15; ++i) t.insert(i);
    t.print();
    std::cout << "**************\n";
    // std::queue<AVLTree<int>::Node*> q;
    // q.push(t.root);
    // while (q.size())
    // {
    //     auto* tmp = q.front();
    //     std::cout << tmp->key << ' ';
    //     if (tmp->left) q.push(tmp->left);
    //     if (tmp->right) q.push(tmp->right);
    //     q.pop(); 
    // }
    std::queue<std::pair<AVLTree<int>::Node*, int>> q;
    q.push(std::pair<AVLTree<int>::Node*, int>(t.root, 0));
    while (q.size())
    {
        auto* tmp = q.front().first;
        int depth = q.front().second;
        std::cout << tmp->key << ' ';
        q.pop();
        if (tmp->left) q.push(std::pair<AVLTree<int>::Node*, int>(tmp->left, depth + 1));
        if (tmp->right) q.push(std::pair<AVLTree<int>::Node*, int>(tmp->right, depth + 1));
        if (q.size() && q.front().second != depth) std::cout << '\n';
    }
}
