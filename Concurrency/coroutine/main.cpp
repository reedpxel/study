#include <iostream>
#include <functional>
#include <cassert>

#include "coroutine.hpp"

void testResume()
{
    std::string str;
    Coroutine coro([&str](Coroutine*) { str.push_back('2'); });
    str.push_back('1');
    coro.resume();
    str.push_back('3');
    assert(str == "123");
    std::cout << "testResume passed" << std::endl;
}

void testSuspend()
{
    std::string str;
    Coroutine coro([&str](Coroutine* self)
        {
            str.push_back('2');
            ThisCoro::suspend(self);
            str.push_back('4');
        });
    str.push_back('1');
    coro.resume();
    str.push_back('3');
    coro.resume();
    assert(coro.isDone());
    str.push_back('5');
    assert(str == "12345");
    std::cout << "testSuspend passed" << std::endl;
}

std::string test1Str;
void f(Coroutine* self) 
{ 
    test1Str.push_back('b');
    ThisCoro::suspend(self);
    test1Str.push_back('d');
}

void testCStyleFunction()
{
    Coroutine coro(f);
    test1Str.push_back('a');
    coro.resume();
    test1Str.push_back('c');
    coro.resume();
    assert(test1Str == "abcd");
    std::cout << "testCStyleFunction passed" << std::endl;
}

void testFunctionalObject()
{
    struct Functor
    {
        void operator()(Coroutine*) const {}
    };

    Functor functor;
    Coroutine coro(functor);
    coro.resume();
    assert(coro.isDone());
    std::cout << "testFunctionalObject passed" << std::endl;
}

void testLambda()
{
    std::string str;
    auto l = [&str](Coroutine* self)
    {
        str.push_back('2');
        ThisCoro::suspend(self);
        str.push_back('4');
    };
    Coroutine coro(l);
    str.push_back('1');
    coro.resume();
    str.push_back('3');
    coro.resume();
    assert(str == "1234");
    std::cout << "testLambda passed" << std::endl;
}

std::string res;
void h(Coroutine* self, int x, double y) 
{
    res += std::to_string(x);
    ThisCoro::suspend(self);
    res += std::to_string(y);
}

void testBind()
{
    Coroutine coro{std::bind(h, std::placeholders::_1, 1, 2.3)};
    res += "x is ";
    coro.resume();
    res += ", y is ";
    coro.resume();
    res += ".";
    assert(res == "x is 1, y is 2.300000.");
    std::cout << "testBind passed" << std::endl;
}

struct TreeNode
{
    char ch;
    TreeNode* left;
    TreeNode* right;
};

struct TreeIterator
{
    TreeIterator(TreeNode* node)
            : node_(node)
            , coro([this](Coroutine* self) 
                {
                    traverseTree(this->node_, self);
                })
    {
        coro.resume();
    }

    TreeIterator& operator++()
    {
        coro.resume();
        return *this;
    }

    char operator*()
    {
        return node_->ch;
    }

    bool isEndIterator()
    {
        return coro.isDone();
    }
private:
    void traverseTree(TreeNode* currentNode, Coroutine* coro)
    {
        if (!currentNode)
        {
            return;
        }
        traverseTree(currentNode->left, coro);
        node_ = currentNode;
        ThisCoro::suspend(coro);
        traverseTree(currentNode->right, coro);
    }
private:
    TreeNode* node_;
    Coroutine coro;
};

void testTreeTraversal()
{
    TreeNode node1{'1', nullptr, nullptr};
    TreeNode node2{'2', nullptr, nullptr};
    TreeNode node3{'3', nullptr, nullptr};
    TreeNode node4{'4', nullptr, nullptr};
    TreeNode node5{'5', nullptr, nullptr};
    TreeNode node6{'6', nullptr, nullptr};
    TreeNode node7{'7', nullptr, nullptr};
    TreeNode node8{'8', nullptr, nullptr};
    node1.left = &node2;
    node1.right = &node3;
    node2.left = &node4;
    node2.right = &node5;
    node3.left = &node6;
    node3.right = &node7;
    node5.right = &node8;
/*
 *      1
 *     / \
 *    2   3
 *   /\  / \
 *  4  5 6  7
 *     \
 *     8
 * */
    TreeIterator it{&node1};
    std::string traversalResult;
    while (!it.isEndIterator())
    {
        traversalResult.push_back(*it);
        ++it;
    }
    assert(traversalResult == "42581637");
    std::cout << "testTreeTraversal passed" << std::endl;
}

void runAllTests()
{
    testResume();
    testSuspend();
    testCStyleFunction();
    testFunctionalObject();
    testLambda();
    testBind();
    testTreeTraversal();

    std::cout << "\nAll tests passed" << std::endl;
}

int main()
{
    runAllTests();
}

