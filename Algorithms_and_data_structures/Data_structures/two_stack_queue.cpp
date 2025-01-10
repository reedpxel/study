#include <iostream>
#include <stack>

struct TwoStacksQueue{
    std::stack<int> _in, _out;
    TwoStacksQueue(){
        std::stack<int> _in;
        std::stack<int> _out;
    }
    bool IsEmpty(){
        return _in.empty() && _out.empty();
    }
    void Push(int _value){
        _in.push(_value);
    }
    void Shift(){
        while(!_in.empty()){
            _out.push(_in.top());
            _in.pop();
        }
    }
    void Pop(){
        if (_out.empty())
            Shift();
        _out.pop();
    }
    int Size(){
        return _in.size() + _out.size();
    }
    int Top(){
        if (!(_in.empty() && _out.empty())){
            if (_out.empty())
                Shift();
            return _out.top();
        }
        return -1;
    }
};

int main(){
    TwoStacksQueue tsq;
    std::cout << tsq.IsEmpty() << '\n';
    std::cout << tsq.Top() << '\n';
    std::cout << tsq.IsEmpty();
    return 0;
}
