#include <iostream>
#include <vector>
#include <stack>

/**
 * Euler path search algorithm. A check that the graph has an euler path 
 * (deg_in == deg_out - number of incoming edges is equal to number of 
 * outcoming edges for every vertice) must be done before the search.
 */

using namespace std;

vector<int> getEulerPath(const vector<vector<bool>>& adjMatrix)
{
    stack<int> st;
    st.push(0);
    vector<int> path;
    vector<vector<bool>> edgeVisited(adjMatrix.size(), 
        vector<bool>(adjMatrix.size()));
    while (!st.empty())
    {
        int curVertice = st.top();
        bool edgeFound = false;
        for (int i = 0; i < adjMatrix.size(); ++i)
        {
            if (adjMatrix[curVertice][i] && !edgeVisited[curVertice][i])
            {
                st.push(i);
                edgeVisited[curVertice][i] = true;
                edgeFound = true;
                break;
            }
        }
        if (!edgeFound)
        {
            st.pop();
            path.push_back(curVertice);
        }
    }
    return vector<int>(path.rbegin(), path.rend());
}

int main()
{
    vector<vector<bool>> adjMatrix = 
    {
        {1, 1, 0, 0},
        {0, 0, 1, 1},
        {1, 1, 0, 0},
        {0, 0, 1, 1}
    };
    vector<int> eulerPath = getEulerPath(adjMatrix);
    for (int x : eulerPath)
    {
        std::cout << x << ' ';
    }
    std::cout << '\n';
}
