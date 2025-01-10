#include <iostream>

class BinarySearchTree
{
    struct Node
    {
        int data;
        Node* left;
        Node* right;
        Node(int data, Node* left, Node* right): data(data), left(left), right(right){}
    };

    Node* root;

    Node* Find(Node* node, int value)
    {
        if (!node){ return nullptr; }
        if (node->data == value){ return node; }
        if (node->data > value){ return Find(node->left, value); }
        return Find(node->right, value);
    }

    std::pair<Node*, bool> FindParent(Node* node, int child_value) // left - true, right - false
    {
        std::pair<Node*, bool> p = {nullptr, true};
        if (node->data == child_value) return p; // if the node is a root, method returns {nullptr, does not matter}
        if (node->left->data == child_value)
        {
            p.first = node;
            return p;
        }
        if (node->right->data == child_value)
        {
            p.first = node;
            p.second = false;
            return p;
        }
        if (child_value < node->data) return FindParent(node->left, child_value);
        return FindParent(node->right, child_value);
    }

    void Insert(Node* nodeToInsert, Node* currentRoot)
    {
        if (nodeToInsert->data == currentRoot->data) return;
        else if (nodeToInsert->data < currentRoot->data)
        {
            if (!currentRoot->left)
            {
                currentRoot->left = nodeToInsert;
                return;
            }
            Insert(nodeToInsert, currentRoot->left);
        } else {
            if (!currentRoot->right)
            {
                currentRoot->right = nodeToInsert;
                return;
            }
            Insert(nodeToInsert, currentRoot->right);
        }
    }

    void TraversePreOrder(Node* node)
    {
        if (!node) return;
        std::cout << (node->data) << ' ';
        TraversePreOrder(node->left);
        TraversePreOrder(node->right);
    }

    void TraverseInOrder(Node* node)
    {
        if (!node) return;
        TraverseInOrder(node->left);
        std::cout << (node->data) << ' ';
        TraverseInOrder(node->right);
    }

    void TraversePostOrder(Node* node)
    {
        if (!node) return;
        TraversePostOrder(node->left);
        TraversePostOrder(node->right);
        std::cout << (node->data) << ' ';
    }

    void DeleteNotWithTwoChildren(Node* ptr, std::pair<Node*, bool> parent)
    {
        if (!ptr->left)
        {
            if (!ptr->right) // the element to be deleted is a leaf
            {
                if (parent.second)
                {
                    parent.first->left = nullptr;
                } else {
                    parent.first->right = nullptr;
                }
                delete ptr;
                return;
            } else { // the element to be deleted has only a right child
                if (parent.second)
                {
                    parent.first->right = ptr->right;
                } else {
                    parent.first->left = ptr->right;
                }
                delete ptr;
                return;
            }
        }
        if (!ptr->right) // the element to be deleted has only a left child
        {
            if (parent.second)
            {
                parent.first->left = ptr->left;
            } else {
                parent.first->right = ptr->left;
            }
            delete ptr;
            return;
        }
    }

    Node* Min(Node* node)
    {
        if (!node->left) return node;
        return Min(node->left);
    }

    Node* Max(Node* node)
    {
        if (!node->right) return node;
        return Max(node->right);
    }

public:

    BinarySearchTree(): root(nullptr) {}

    BinarySearchTree(Node* root): root(root) {}

    Node* Find(int value){ return Find(root, value); }

    void Insert(int value)
    {   
        Node* node = new Node(value, nullptr, nullptr);
        if (!root)
        {
            root = node;
            return;
        }
        Insert(node, root);
    }

    void TraversePreOrder(){ TraversePreOrder(root); }

    void TraverseInOrder(){ TraverseInOrder(root); }

    void TraversePostOrder(){ TraversePostOrder(root); }

    void Traverse(){ TraverseInOrder(root); }

    void Erase(int value)
    {
        Node* ptr = Find(value);
        if (!ptr) return;
        if (!ptr->left || !ptr->right)
        {
            std::pair<Node*, bool> pair = FindParent(root, ptr->data);
            DeleteNotWithTwoChildren(ptr, pair);
            return;
        }
        Node* smallestBigger = Min(ptr->right);
        std::pair<Node*, bool> pair = FindParent(root, smallestBigger->data);
        int tmp = ptr->data;
        ptr->data = smallestBigger->data;
        smallestBigger->data = tmp;
        DeleteNotWithTwoChildren(smallestBigger, pair);
    }

    Node* Min() { return Min(root); }

    Node* Max() { return Max(root); }
};

int main()
{
    BinarySearchTree t;
    t.Insert(9);
    t.Insert(1);
    t.Insert(11);
    t.Insert(4);
    t.Insert(3);
    t.Insert(10);
    t.Insert(14);
    t.Insert(0);
    t.Traverse();
    t.Erase(11);
    std::cout << '\n';
    t.Traverse();
    return 0;
}