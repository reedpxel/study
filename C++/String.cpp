#include <iostream>
#include <cstring>
#include <iomanip>
#define INPUT_BUFFER_SIZE 1000

class String{
private:
    unsigned long long _size = 0; //_size is the actual size of the string, excluding '\0' at the end
    char* str = nullptr;
public:
    String() = default;
    String(const String& s): _size(s._size), str(new char[_size+1]){
        memcpy(str, s.str, _size+1);
    }
    String(std::initializer_list<char> list): _size(list.size()), str(new char[_size+1]){
        std::copy(list.begin(), list.end(), str);
        str[_size] = '\0';
    }
    String (const char* _str): _size(strlen(_str)), str(new char[_size+1]){
        memcpy(str, _str, _size+1);
    }
    String(char ch, unsigned long long sz): _size(sz), str(new char[_size+1]){
        for (int i=0; i<_size; ++i)
            str[i] = ch;
        str[_size] = '\0';
    }
    ~String(){
        delete[] str;
    }
    String& operator = (const String& s){
        if (this==&s)
            return *this;
        delete[] str;
        _size = s._size;
        str = new char[_size+1];
        memcpy(str, s.str, _size+1);
        return *this;
    }
    bool operator!() const { return _size==0; }
    bool operator==(const String& s) const { return !strcmp(str, s.str); }
    bool operator!=(const String& s) const { return !(*this==s); }
    bool operator<(const String& s) const { return this->_size<s._size; }
    bool operator>(const String& s) const { return this->_size>s._size; }
    bool operator<=(const String& s) const { return this->_size<=s._size; }
    bool operator>=(const String& s) const { return this->_size>=s._size; }
    String& operator+=(const String& s){
        if (!s)
            return *this;
        char* newstr = new char[_size];
        memcpy(newstr, str, _size);
        memcpy((newstr+_size), s.str, s._size+1);
        _size+=s._size;
        str = newstr;
        return *this;
    }
    char& operator [] (unsigned long long i){ return str[i]; }
    const char& operator[](unsigned long long i) const { return str[i]; }
    String operator()(unsigned long long begin, unsigned long long end) const {
        char tmp[end-begin+2];
        memcpy(tmp, str+begin, end-begin+1);
        tmp[end-begin+1] = '\0';
        String* res = new String(tmp);
        return *res;
    }
    unsigned long long size() const { return _size; }
    friend std::ostream& operator << (std::ostream& output, const String& s){
        output << s.str;
        return output;
    }
    friend std::istream& operator >> (std::istream& input, String& s){
        char tmp[INPUT_BUFFER_SIZE];
        input >> std::setw(INPUT_BUFFER_SIZE) >> tmp; // setw reduces the input string, if it is too long
        s = tmp;
        return input;
    }
};

int main(){
    const String s1 = "aasdf";
    const String s2 = s1(0, 2);
    std::cout << s2 << ' ' << s2.size();
    return 0;
}
