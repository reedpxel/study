#include <iostream>
#include <vector>
#include <cstring>
#include <queue>
#define SIGMA 26 // размерность алфавита

/*
Решение задачи поиска входжения множества паттернов в текст при помощи алгоритма Ахо-Корасик. 
    - построение бора за O(|SIGMA| * сумма длин всех паттернов)
    - переходы по сжатым ссылкам бора за O(кол-ва терминалов = числу паттернов)
    - кол-во этих переходов - O(ans)
Для случая одного паттерна алгоритм вырождается в алгоритм Кнута-Морриса-Прата, бор становится "бамбуком"
*/

class Trie
{
public:
    struct Node
    {
        int vertices[SIGMA];
        bool isTerminal;

        Node();
    };

    int count;
    std::vector<Node> to;

    Trie();
    void add(const std::string& str, std::vector<int>& terminalsToPatterns);
};

Trie::Node::Node() : isTerminal(false)
{
    memset(vertices, -1, sizeof(vertices));
}

Trie::Trie() : count(0), to(std::vector<Node>(1, Node())) {}

void Trie::add(const std::string& str, std::vector<int>& terminalsToPatterns)
{
    int currentNode = 0;
    for (int i = 0; i < str.size(); ++i)
    {
        if (to[currentNode].vertices[str[i] - 'a'] < 0)
        {
            to.push_back(Node());
            to[currentNode].vertices[str[i] - 'a'] = to.size() - 1;
        }
        currentNode = to[currentNode].vertices[str[i] - 'a'];
    }
    to[currentNode].isTerminal = true;
    if (terminalsToPatterns.size() <= currentNode) terminalsToPatterns.resize(currentNode + 1);
    terminalsToPatterns[currentNode] = count;
    ++count;
}

int CompressedLink(int vertice, const Trie& trie, const std::vector<int>& link)
{
    if (!link[vertice] || trie.to[link[vertice]].isTerminal) return link[vertice];
    return CompressedLink(link[vertice], trie, link);
}

std::vector<std::vector<int>> get_patterns(const std::vector<std::string>& patterns, 
    const std::string& text)
{
    // построение бора
    Trie trie;
    std::vector<int> terminalsToPatterns;
    for (auto str : patterns) trie.add(str, terminalsToPatterns);
    std::vector<int> link(trie.to.size());
    std::vector<std::vector<int>> go(trie.to.size(), std::vector<int>(SIGMA));
    std::queue<int> bfsQueue;
    bfsQueue.push(0);
    while (!bfsQueue.empty())
    {
        int currentVertice = bfsQueue.front();
        bfsQueue.pop();
        for (int i = 0; i < SIGMA; ++i)
        {
            for (int letter = 0; letter < SIGMA; ++letter)
            {
                if (trie.to[currentVertice].vertices[letter] > 0)
                {
                    go[currentVertice][letter] = trie.to[currentVertice].vertices[letter];
                } else {
                    go[currentVertice][letter] = go[link[currentVertice]][letter];
                }
            }
            if (trie.to[currentVertice].vertices[i] > 0)
            {
                int newVertice = trie.to[currentVertice].vertices[i];
                bfsQueue.push(newVertice);
                link[newVertice] = currentVertice == 0 ? 0 : go[link[currentVertice]][i];
            }
        }
    }

    // for (auto x : link) std::cout << x << ' ';
    // std::cout << '\n';
    // for (int i = 0; i < go.size(); ++i)
    // {
    //     std::cout << i << "  ";
    //     for (int j = 0; j < 3; ++j)
    //     {
    //         std::cout << go[i][j] << ' ';
    //     }
    //     std::cout << '\n';
    // }

    // получение ответа
    std::vector<std::vector<int>> res(patterns.size());
    // for (int a = 0; a < trie.to.size(); ++a) std::cout << CompressedLink(a, trie, link) << ' ';
    int nextVertice = 0;
    for (int i = 0; i < text.size(); ++i)
    {
        nextVertice = go[nextVertice][text[i] - 'a'];
        int tmpVertice = nextVertice;
        for (int cycle = 0; cycle <= trie.to.size(); ++cycle)
        {
            if (trie.to[tmpVertice].isTerminal)
            {
                res[terminalsToPatterns[tmpVertice]].push_back(i - 
                    patterns[terminalsToPatterns[tmpVertice]].size() + 1);
            }
            tmpVertice = CompressedLink(tmpVertice, trie, link);
            if (!tmpVertice) break;
        }
    }
    return res;
}

// Trie on map
// переход по ребру за O(log(k)), создание ребра за O(log(k)), создание вершины за O(1)
// массив быстрее, использовать map имеет смысл, если экономим память и граф разрежен

class TrieOnMap
{
public:
    struct Node
    {
        std::map<char, Node*> to;
        bool isTerminal;

        Node();
    };
private:
    void destructor_helper(Node* node);
    void print_helper(Node* node);
public:
    Node* root;

    TrieOnMap();
    void add(const std::string& str);
    ~TrieOnMap();
    void print();
};

TrieOnMap::Node::Node() : to(std::map<char, Node*>()), isTerminal(false) {}

void TrieOnMap::destructor_helper(Node* node)
{
    for (auto x : node->to) destructor_helper(x.second);
    delete node;
}

TrieOnMap::TrieOnMap() : root(new Node()) {}

void TrieOnMap::add(const std::string& str)
{
    if (str.empty()) return;
    Node* currentNode = root;
    for (int i = 0; i < str.size(); ++i)
    {
        auto itFind = currentNode->to.find(str[i]);
        if (itFind == currentNode->to.end())
        {
            Node* pNewNode = new Node;
            currentNode->to.insert(std::make_pair(str[i], pNewNode));
            currentNode = pNewNode;
        } else {
            currentNode = (*itFind).second;
        }
    }
    currentNode->isTerminal = true;
}

TrieOnMap::~TrieOnMap()
{
    destructor_helper(root);
}

void TrieOnMap::print() { print_helper(root); }

void TrieOnMap::print_helper(Node* node)
{
    for (auto pair : node->to) std::cout << pair.first << '_';
    std::cout << node->isTerminal << '\n';
    for (auto pair : node->to) print_helper(pair.second);
}

std::vector<int> buildAutomat(const std::vector<std::string>& patterns)
{
    Trie trie;
    for (const std::string& str : patterns) trie.add(str);
    std::vector<int> link(trie.vertices.size());
    std::vector<std::vector<int>> go(trie.vertices.size(), std::vector<int>(SIGMA));

    std::queue<int> bfsQueue;
    bfsQueue.push(0);
    while (!bfsQueue.empty())
    {
        int currentVertice = bfsQueue.front();
        bfsQueue.pop();
        for (int i = 0; i < SIGMA; ++i)
        {
            if (currentVertice.)
        }
    }

    for (auto x : link) std::cout << x << ' ';
    std::cout << '\n';
    for (auto _v : go)
    {
        for (auto x : _v)
        {
            std::cout << x << '_';
        }
        std::cout << '\n';
    }
    return {};
}

int main()
{
    std::vector<std::string> words = 
    {
        "abc"
    };
    std::string text = "cbdabcaaaccbdccbadcbbdcb";
    auto res = get_patterns(words, text);
    for (int i = 0; i < res.size(); ++i)
    {
        std::cout << i << ' ';
        for (auto x : res[i])
        {
            std::cout << x << ' ';
        }
        std::cout << '\n';
    }
}
