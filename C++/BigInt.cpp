#include <iostream>
#include <iomanip>
#include <cstring>
#include <cmath>
#define INPUT_BUFFER_SIZE 1000

class BigInt{

    std::string str;

    //inner help functions
    static std::string add(const char* s1, const char* s2){
        std::string res;
        const char* added_array;
        if (strlen(s1) >= strlen(s2)){
            res = s1;
            added_array = s2;
        } else {
            res = s2;
            added_array = s1;
        }
        bool carry = false;
        size_t bigger_size = res.size();
        size_t smaller_size = strlen(added_array);
        for (size_t i = smaller_size; i > 0; --i){
            res[i + bigger_size - smaller_size - 1] += added_array[i - 1] - 48;
            if (carry){
                ++res[i + bigger_size - smaller_size - 1];
            }
            if (res[i + bigger_size - smaller_size - 1] > 57){
                carry = true;
                res[i + bigger_size - smaller_size - 1] -= 10;
            } else {
                carry = false;
            }
        }
        for (size_t i = bigger_size - smaller_size; i > 0; --i){
            if (!carry){
                break;
            }
            ++res[i - 1];
            if (res[i - 1] == 58){
                res[i - 1] = 48;
            } else {
                carry = false;
            }
        }
        if (carry){
            res.insert(0, "1");
        }
        return res;
    }

    static std::string substract(const char* s1, const char* s2){
        std::string res = s1;
        size_t s1sz = strlen(s1), s2sz = strlen(s2);
        bool carry = false;
        for (size_t i = strlen(s2); i > 0; --i){
            res[i + s1sz - s2sz - 1] -= s2[i - 1] - 48;
            if (res[i + s1sz - s2sz - 1] < 48){
                carry = true;
                res[i + s1sz - s2sz - 1] += 10;
                for (size_t j = s1sz -s2sz + i - 1; j > 0; --j){
                    --res[j - 1];
                    if (res[j - 1] == 47){
                        res[j - 1] = 57;
                    } else {
                        carry = false;
                        break;
                    }
                }
            }
        }
        if (res[0] == 48){
            res.erase(0, res.find_first_not_of('0'));
            if (res == ""){
                res = "0";
            }
        }
        return res;
    }

    static std::string multiply(const char* _s1, const char* _s2){
        if (_s1 == "0" || _s2 == "0")
            return "0";
        std::string res = "0";
        size_t s1sz = strlen(_s1), s2sz = strlen(_s2);
        unsigned char carry = 0;
        for (size_t i=s2sz; i>0; --i){
            char res_tmp[s1sz+2+s2sz-i]{};
            for (size_t j=s1sz+s2sz-i; j>s1sz; --j)
                res_tmp[j] = 48;
            for (size_t j=s1sz; j>0; --j){
                unsigned char tmp = (_s1[j-1]-48)*(_s2[i-1]-48) + carry;
                res_tmp[j] = tmp%10+48;
                carry = tmp/10;
            }
            res_tmp[0] = carry+48;
            carry = 0;
                res = add(res.c_str(), res_tmp);
        }
        res.erase(0, res.find_first_not_of('0'));
        return res;
    }

    static std::string divide_by_two(const std::string& _s){
        char res[_s.size()+1]{};
        unsigned char remainder = 0;
        for (size_t i=0; i<_s.size(); ++i){ 
            unsigned char tmp = remainder*10+_s[i]-48;
            res[i] = tmp/2+48;
            remainder = tmp%2;
        }
        res[_s.size()+1] = 0;
        std::string str_res = std::string(res);
        if (str_res[0]==48 && str_res.size()>1)
            str_res.erase(0, 1);
        return str_res;
    }

    static std::string divide(const char* _s1, const char* _s2){
        if (BigInt(_s1)<BigInt(_s2)){
            return "0";
        }
        size_t s1sz = strlen(_s1), s2sz = strlen(_s2);
        char _lower_boundary[s1sz-s2sz+2], _upper_boundary[s1sz-s2sz+2];
        _lower_boundary[0] = '1';
        for(size_t i=1; i<s1sz-s2sz; ++i)
            _lower_boundary[i] = '0';
        _lower_boundary[s1sz-s2sz] = '\0';
        _lower_boundary[s1sz-s2sz+1] = '\0';
        for (size_t i=0; i<s1sz-s2sz+1; ++i)
            _upper_boundary[i] = '9';
        _upper_boundary[s1sz-s2sz+1] = '\0';
        while (true){
            if (BigInt(multiply(_lower_boundary, _s2))<=BigInt(_s1) && 
                BigInt(multiply(add(_lower_boundary, "1").c_str(), _s2))>BigInt(_s1))
                break;
            std::string tmp = divide_by_two(add(_upper_boundary, _lower_boundary));
            if (BigInt(multiply(tmp.c_str(), _s2))<=BigInt(_s1)){
                strcpy(_lower_boundary, tmp.c_str());
            }
            else{
                strcpy(_upper_boundary, tmp.c_str());
            }
        }
        return _lower_boundary;
    }

public:

    BigInt(): str("0") {}

    BigInt(const std::string& _str): str(_str) {}

    BigInt(const char* _ch): str(std::string(_ch)) {}

    BigInt(int _i): str(std::to_string(_i)) {}

    bool operator<(const BigInt& _bi) const {
        if (str[0] == '-'){
            if (_bi.str[0] == '-'){
                if (str.size()==_bi.str.size()){
                    for (size_t i = 1; i<str.size(); ++i){
                        if (str[i]<_bi.str[i]){
                            return false;
                        }
                        if (str[i]>_bi.str[i]){
                            return true;
                        }
                    }
                } else {
                    return str.size()>_bi.str.size();
                }
            } else {
                return true;
            }
        } else {
            if (_bi.str[0] == '-'){
                return false;
            } else {
                if (str.size()==_bi.str.size()){
                    for (size_t i = 0; i<str.size(); ++i){
                        if (str[i]<_bi.str[i]){
                            return true;
                        }
                        if (str[i]>_bi.str[i]){
                            return false;
                        }
                    }
                } else {
                    return str.size()<_bi.str.size();
                }
            }
        }
        return false;
    }

    //comparison operators
    bool operator>(const BigInt& _bi) const { return _bi < *this; }

    bool operator==(const BigInt& _bi) const { return !(*this > _bi || *this < _bi); }

    bool operator!=(const BigInt& _bi) const { return *this > _bi || *this < _bi; }

    bool operator<=(const BigInt& _bi) const { return !(*this > _bi); }

    bool operator>=(const BigInt& _bi) const { return !(*this < _bi); }

    //arithmetic operations

    BigInt& operator+=(const BigInt& _bi){
        std::string res;
        if (str[0] == '-'){
            if (_bi.str[0] == '-'){
                res = add(str.c_str() + 1, _bi.str.c_str() + 1);
                res.insert(0, "-");
            } else {
                if (BigInt(str.c_str() + 1) >= _bi){
                    res = substract(str.c_str() + 1, _bi.str.c_str());
                    if (res!="0"){
                        res.insert(0, "-");
                    }
                } else {
                    res = substract(_bi.str.c_str(), str.c_str() + 1);
                }
            }
        } else {
            if (_bi.str[0] == '-'){
                if (*this >= BigInt(_bi.str.c_str() + 1)){
                    res = substract(str.c_str(), _bi.str.c_str() + 1);
                } else {
                    res = substract(_bi.str.c_str() + 1, str.c_str());
                    if (res!="0"){
                        res.insert(0, "-");
                    }
                }
            } else {
                res = add(str.c_str(), _bi.str.c_str());
            }
        }
        str = res;
        return *this;
    }

    BigInt operator+(const BigInt& _bi) const {
        BigInt res = *this;
        return res+=_bi;
    }

    BigInt& operator++(){ return *this+=1; }

    BigInt operator++(int){
        BigInt res = *this;
        res+=1;
        return res;
    }

    BigInt& operator-=(const BigInt& _bi){
        std::string res;
        if (str[0]=='-'){
            if (_bi.str[0] == '-'){
                if (BigInt(str.c_str() + 1) >= BigInt(_bi.str.c_str() + 1)){
                    res = substract(str.c_str() + 1, _bi.str.c_str() + 1);
                    if (res!="0"){
                        res.insert(0, "-");
                    }
                } else {
                    res = substract(_bi.str.c_str() + 1, str.c_str() + 1);
                }
            } else {
                res = add(str.c_str() + 1, _bi.str.c_str());
                if (res!="0"){
                    res.insert(0, "-");
                }
            }
        } else {
            if (_bi.str[0] == '-'){
                res = add(str.c_str(), _bi.str.c_str() + 1);
            } else {
                if (BigInt(str.c_str()) >= BigInt(_bi.str.c_str())){
                    res = substract(str.c_str(), _bi.str.c_str());
                } else {
                    res = substract(_bi.str.c_str(), str.c_str());
                    if (res!="0"){
                        res.insert(0, "-");
                    }
                }
            }
        }
        str = res;
        return *this;
    }

    BigInt operator-(const BigInt& _bi){
        BigInt res = *this;
        return res -= _bi;
    }

    BigInt& operator--(){ return *this-=1; }

    BigInt operator--(int){
        BigInt res = *this;
        res += 1;
        return res;
    }

    BigInt& operator*=(const BigInt& _bi){
        std::string res;
        if (str[0] == '-'){
            if (_bi.str[0] == '-'){
                res = multiply(str.c_str() + 1, _bi.str.c_str() + 1);
            } else {
                res = multiply(str.c_str() + 1, _bi.str.c_str());
                if (res != "0"){
                    res.insert(0, "-");
                }
            }
        } else {
            if (_bi.str[0] == '-'){
                res = multiply(str.c_str(), _bi.str.c_str() + 1);
                if (res != "0"){
                    res.insert(0, "-");
                }
            } else {
                res = multiply(str.c_str(), _bi.str.c_str());
            }
        }
        str = res;
        return *this;
    }

    BigInt operator*(const BigInt& _bi){
        BigInt res = *this;
        return res *= _bi;
    }

    BigInt& operator/=(const BigInt& _bi){
        std::string res;
        if (_bi == "0"){
            str = "";
            return *this;
        }
        if (str[0] == '-'){
            if (_bi.str[0] == '-'){
                res = divide(str.c_str() + 1, _bi.str.c_str() + 1);
            } else {
                res = divide(str.c_str() + 1, _bi.str.c_str());
                if (res != "0"){
                    res.insert(0, "-");
                }
            }
        } else {
            if (_bi.str[0] == '-'){
                res = divide(str.c_str(), _bi.str.c_str() + 1);
                if (res != "0"){
                    res.insert(0, "-");
                }
            } else {
                res = divide(str.c_str(), _bi.str.c_str());
            }
        }
        str = res;
        return *this;
    }

    BigInt operator/(const BigInt& _bi){
        BigInt res = *this;
        return res /= _bi;
    }

    BigInt& operator%=(const BigInt& _bi){
        if (_bi == "0"){
            *this = "";
            return *this;
        }
        *this = *this - (*this/_bi)*_bi;
        return *this;
    }

    BigInt operator%(const BigInt& _bi){
        BigInt res = *this;
        return res %= _bi;
    }

    //input and output operators
    friend std::istream& operator>>(std::istream& input, BigInt& _bi){
        char tmp[INPUT_BUFFER_SIZE];
        input >> std::setw(INPUT_BUFFER_SIZE) >> tmp;
        _bi = tmp;
        return input;
    }

    friend std::ostream& operator<<(std::ostream& output, const BigInt& _bi){
        output << _bi.str;
        return output;
    }

    // static public functions
    static BigInt abs(const BigInt& bi){
        BigInt res = bi;
        if (res.str[0] == '-'){
            res.str.erase(0, 1);
        }
        return res;
    }

    static BigInt pow(const BigInt& base, const BigInt& power){
        BigInt res = base;
        if (power < "0"){
            res = "0";
            return res;
        }
        if (power == "0"){
            res = "1";
            return res;
        }        
        for (BigInt i = "0"; i < power; ++i){
            res *= base;
        }
        return res;
    }
};

int main(){
    BigInt b1 = "40";
    BigInt b2 = "50";
    std::cout << b1 * b2;
}
