#include <iostream>
#include <random>

template <typename T>
class RedBlackTree
{
    enum {RED = true, BLACK = false};

    struct Node
    {
        T key;
        bool color;
        Node* left;
        Node* right;
        Node* parent;
        Node(T _key, bool _color = RED);
        Node(T _key, Node* _parent, bool _color = RED);
        Node();
    };

    Node* root;
    size_t sz;
    Node null_node;

    void clear_rec(Node* node);
    void print_rec(Node* node, int depth) const;
    void rotate_left(Node* node);
    void rotate_right(Node* node);
    void balance_after_insertion(Node* x);
    void balance_after_removal(Node* node, bool _left);
public:
    RedBlackTree();
    ~RedBlackTree();
    void print() const;
    void clear();
    size_t size() const;
    Node* find(T _key) const;
    bool contains(T _key) const;
    bool insert(T _key);
    bool remove(T _key);
};

template <typename T>
RedBlackTree<T>::Node::Node(T _key, bool _color) : key(_key), color(_color), left(nullptr), right(nullptr), parent(nullptr) {}

template <typename T>
RedBlackTree<T>::Node::Node(T _key, Node* _parent, bool _color) : key(_key), color(_color), left(nullptr), right(nullptr), parent(_parent) {}

template <typename T>
RedBlackTree<T>::Node::Node() : key(INT_MIN), color(BLACK), left(nullptr), right(nullptr), parent(nullptr) {}

template <typename T>
void RedBlackTree<T>::clear_rec(Node* node)
{
    if (!node || node->key == INT_MIN) return;
    clear_rec(node->left);
    clear_rec(node->right);
    delete node;
}

template <typename T>
void RedBlackTree<T>::print_rec(Node* node, int depth) const
{
    if (!node) return;
    print_rec(node->right, depth + 1);
    for (int i = 0; i < depth; ++i) std::cout << '\t';
    if (node->key == INT_MIN) std::cout << "null\n";
    else 
    {
        std::cout << node->key << ' ';
        if (!node->parent) std::cout << "nullptr ";
        else std::cout << node->parent->key << ' ';
        if (node->color == RED) std::cout << "r\n";
        else std::cout << "b\n";
    }
    print_rec(node->left, depth + 1);
}

template <typename T>
void RedBlackTree<T>::rotate_left(Node* node)
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
}

template <typename T>
void RedBlackTree<T>::rotate_right(Node* node)
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
}

template <typename T>
void RedBlackTree<T>::balance_after_insertion(Node* x)
{
    if (root->color == RED) root->color = BLACK;
    if (x->parent && x->color == RED && x->parent->color == RED)
    {
        Node* y = x->parent;
        Node* g = y->parent; // тут мб seg fault
        Node* u = y == g->left ? g->right : g->left;
        if (u->color == RED)
        {
            // случай 1
            g->color = RED;
            u->color = BLACK;
            y->color = BLACK;
        } else {
            // случаи 2
            if (u == g->right)
            {
                if (x == y->right) // если выполняется, случай 2.2, нет - 2.1
				{
					rotate_left(y);
					y = x;
				} 
                rotate_right(g);
            } else {
                if (x == y->left) // то же, но зеркальное отражение
				{
					rotate_right(y);
					y = x;
				}
                rotate_left(g);
            }
            y->color = BLACK;
            g->color = RED;
        }
        if (x->parent->parent) balance_after_insertion(x->parent->parent);
    }
}

template <typename T>
void RedBlackTree<T>::balance_after_removal(Node* node, bool _left)
{
    if (!node) return;
    if (_left) // поддерево с меньшей черной глубиной - левый сын node
    {
        Node* b = node->right;
        if (node->color == RED) // корень красный
        {
            if (b->left->color == RED)
            {
                rotate_right(b);
                rotate_left(node);
                node->color = BLACK;
				// std::cout << "1 ";
            } else if (b->right->color == RED) {
                rotate_left(node);
				// std::cout << "2 ";
                // node->color = BLACK;
                // b->color = RED; тут была ошибка
            } else {
                node->color = BLACK;
                b->color = RED;
				// std::cout << "3 ";
            }
        } else { // корень черный
            if (b->color == RED) // брат красный
            {
                if (b->left->right && b->left->right->color == RED)
                {
                    rotate_right(b);
                    rotate_left(node); // была ошибка
                    b->left->color = BLACK;
					// std::cout << " 4 ";
                } else if (b->left->left && b->left->left->color == RED) {
                    rotate_right(b->left);
                    rotate_right(b);
                    rotate_left(node);
                    node->parent->color = BLACK;
					// std::cout << "5 ";
                } else {
                    rotate_left(node);
                    b->color = BLACK;
                    node->right->color = RED;
					// std::cout << "6 ";
                }
            } else { // брат черный
                if (b->right->color == RED)
                {
                    rotate_left(node);
                    b->right->color = BLACK;
					// std::cout << "7 ";
                } else if (b->left->color == RED) {
                    rotate_right(b);
                    rotate_left(node);
                    node->parent->color = BLACK;
					// std::cout << "8 ";
                } else {
                    b->color = RED;
					// std::cout << "9 ";
                    if (node->parent) balance_after_removal(node->parent, node->parent->left == node ? true : false);
                }
            }
        }
    } else { // меньшая черная глубина у правого сына
        Node* b = node->left;
        if (node->color == RED)
        {
            if (b->right->color == RED)
            {
                rotate_left(b);
                rotate_right(node);
                node->color = BLACK; // была ошибка 
				// std::cout << "10 ";
            } else if (b->left->color == RED) {
                rotate_right(node);
				// std::cout << "11 ";
                // b->right->color = BLACK;
                // b->color = RED; тут была ошибка
            } else {
                node->color = BLACK;
                b->color = RED;
				// std::cout << "12 ";
            }  
        } else { // корень черный
            if (b->color == RED) // брат красный
            {
                if (b->right->left && b->right->left->color == RED)
                {
                    rotate_left(b);
                    rotate_right(node); // была ошибка
                    b->right->color = BLACK;
					// std::cout << "13 ";
                } else if (b->right->right && b->right->right->color == RED) {
                    rotate_left(b->right);
                    rotate_left(b);
                    rotate_right(node);
                    node->parent->color = BLACK;
					// std::cout << "14 ";
                } else {
                    rotate_right(node);
                    b->color = BLACK;
                    node->left->color = RED;
					// std::cout << "15 ";
                }
            } else { // брат черный
                if (b->left->color == RED)
                {
                    rotate_right(node);
                    b->left->color = BLACK;
					// std::cout << "16 ";
                } else if (b->right->color == RED) {
                    rotate_left(b);
                    rotate_right(node);
                    node->parent->color = BLACK;
					// std::cout << "17 ";
                } else {
                    b->color = RED;
					// std::cout << "18 ";
                    if (node->parent) balance_after_removal(node->parent, node->parent->left == node ? true : false);
                }
            }
        }
    }
}

template <typename T>
RedBlackTree<T>::RedBlackTree() : root(nullptr), sz(0), null_node(Node()) {}

template <typename T>
RedBlackTree<T>::~RedBlackTree() { clear(); }

template <typename T>
void RedBlackTree<T>::print() const
{
	if (!root)
	{
		std::cout << "null\n";
		return;
	}
	print_rec(root, 0);
}

template <typename T>
void RedBlackTree<T>::clear()
{
    clear_rec(root);
    root = nullptr;
}

template <typename T>
size_t RedBlackTree<T>::size() const { return sz; }

template <typename T>
typename RedBlackTree<T>::Node* RedBlackTree<T>::find(T _key) const
{
    Node* tmp = root;
    if (!tmp) return NULL;
    while (_key != tmp->key && 
        ((_key < tmp->key && tmp->left != &null_node) || (_key > tmp->key && tmp->right != &null_node)))
    {
        tmp = _key < tmp->key ? tmp->left : tmp->right;
    }
    if (_key != tmp->key) return NULL;
    return tmp;
}

template <typename T>
bool RedBlackTree<T>::contains(T _key) const { return find(_key); }

template <typename T>
bool RedBlackTree<T>::insert(T _key)
{
    Node* tmp = root;
    if (!tmp)
    {
        Node* new_root = new Node(_key, BLACK);
        new_root->left = &null_node;
        new_root->right = &null_node;
        root = new_root;
		++sz;
        return true;
    }
    while (_key != tmp->key && 
        ((_key < tmp->key && tmp->left->key != INT_MIN) || (_key > tmp->key && tmp->right->key != INT_MIN)))
    {
        tmp = _key < tmp->key ? tmp->left : tmp->right;
    }
    if (_key == tmp->key) return false;
    Node* new_node = new Node(_key, tmp);
	++sz;
    new_node->left = &null_node;
    new_node->right = &null_node;
    if (_key < tmp->key) tmp->left = new_node;
    else tmp->right = new_node;
    balance_after_insertion(new_node);
    return true;
}

template <typename T>
bool RedBlackTree<T>::remove(T _key)
{
    Node* node = find(_key);
    if (!node) return false;
	--sz;
    if (node->left != &null_node && node->right != &null_node)
    // свап с наибольшим узлом левого поддерева, если у удаляемого узла есть оба ребенка
    {
        Node* swap = node->left;
        while (swap->right != &null_node) swap = swap->right;
        bool tmp_color = node->color;
        node->color = swap->color;
        swap->color = tmp_color;
        Node* par = swap->parent;
        if (node == root)
        {
            root = swap;
            root->parent = nullptr;
        }
        else 
        {
            if (node->parent->left == node) node->parent->left = swap;
            else node->parent->right = swap;
        }
        if (node == par)
        {
            swap->parent = node->parent;
            swap->right = node->right;
            swap->right->parent = swap;
            node->right = &null_node;
            node->left = swap->left;
            if (node->left) node->left->parent = node;
            swap->left = node;
            node->parent = swap;
        } else {
            Node* _l = swap->left;
            swap->left = node->left;
            if (swap->left) swap->left->parent = swap;
            node->left = _l;
            if (_l) _l->parent = node;
            swap->right = node->right;
            swap->right->parent = swap;
            par->right = node;
			swap->parent = node->parent; // !!!!!!!!!!!!!!
            node->parent = par;
            node->right = &null_node;
        }
    }
    if (node->color == RED) // случай 1: узел красный, детей нет (иначе была бы разная черная глубина в поддереве с корнем в этом узле)
    {
        if (node->parent->left == node)
        {
            node->parent->left = &null_node;
        } else {
            node->parent->right = &null_node;
        }
        delete node;
        return true;
    } else {
        if (node->left == &null_node && node->right == &null_node) // случай 2: узел черный, детей нет
        {
            if (node == root)
            {
                root = nullptr;
                delete node;
                return true;
            }
            Node* p = node->parent;
            if (p->left == node)
            {
                p->left = &null_node;
                balance_after_removal(p, true);
            } else {
                p->right = &null_node;
                balance_after_removal(p, false);
            }
            delete node;
            return true;
        } else { // случай 3: узел черный, есть 1 красный ребенок
            if (node == root)
            {
                root = node->right == &null_node ? node->left : node->right;
                root->color = BLACK;
                root->parent = nullptr;
                delete node;
                return true;
            }
            if (node->right == &null_node) // ребенок слева
            {
                node->left->color = BLACK;
                node->left->parent = node->parent;
                if (node->parent->left == node)
                {
                    node->parent->left = node->left;
                } else {
                    node->parent->right = node->left;
                }
            } else { // ребенок справа
                node->right->color = BLACK;
                node->right->parent = node->parent;
                if (node->parent->left == node)
                {
                    node->parent->left = node->right;
                } else {
                    node->parent->right = node->right;
                }
            }
            delete node;
            return true;
        }
    }
    return false;
}

int main()
{
	// for (int i = 0; i < 100; ++i)
	// {
	// 	RedBlackTree<int> t;
	// 	for (int i = 0; i < 10000; ++i) t.insert(i);
	// 	std::random_device d;
	// 	for (int i = 0; i < 100000; ++i)
	// 	{
	// 		int x = d() % 10000;
	// 		t.remove(x);
	// 	}
	// 	t.print();
	// }
	RedBlackTree<int> t;
	for (int i = 0; i < 10; ++i) t.insert(5);
	t.remove(5);
	std::cout << t.size();
}
