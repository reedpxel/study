#include "BigInt.h"

std::string BigInt::add(const char* s1, const char* s2){
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

std::string BigInt::substract(const char* s1, const char* s2){
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

std::string BigInt::multiply(const char* _s1, const char* _s2){
    char _null[2] = "0";
    if (!strcmp(_s1, _null) || !strcmp(_s2, _null))
        return "0";
    std::string res = "0";
    size_t s1sz = strlen(_s1), s2sz = strlen(_s2);
    unsigned char carry = 0;
    for (size_t i=s2sz; i>0; --i){
        std::string res_tmp(s1sz+1+s2sz-i, '0');
        for (size_t j=s1sz; j>0; --j){
            unsigned char tmp = static_cast<unsigned char>((_s1[j - 1] - '0') * (_s2[i - 1] - '0')) + carry;
            res_tmp[j] = tmp % 10 + 48;
            carry = tmp / 10;
        }
        res_tmp[0] = static_cast<char>(carry) + '0';
        carry = 0;
        res = add(res.c_str(), res_tmp.c_str());
    }
    res.erase(0, res.find_first_not_of('0'));
    return res;
}

std::string BigInt::divide_by_two(const std::string& _s){
    std::string res(_s.size(), '0');
    unsigned char remainder = 0;
    for (size_t i=0; i<_s.size(); ++i){
        unsigned char tmp = static_cast<unsigned char>(remainder * 10 + _s[i] - 48);
        res[i] = tmp/2+48;
        remainder = tmp%2;
    }
    res[_s.size()+1] = 0;
    std::string str_res = std::string(res);
    if (str_res[0]==48 && str_res.size()>1)
        str_res.erase(0, 1);
    return str_res;
}

std::string BigInt::divide(const char* _s1, const char* _s2){
    if (BigInt(_s1) < BigInt(_s2)){
        return "0";
    }
    size_t s1sz = strlen(_s1), s2sz = strlen(_s2);
    std::string _lower_boundary(s1sz - s2sz, '0');
    std::string _upper_boundary(s1sz - s2sz + 1, '9');
    if (_lower_boundary.empty()) _lower_boundary = "1";
    else _lower_boundary[0] = '1';
    for (int cycle = 0; cycle < 10000; ++cycle){
//    while (true){
        if (BigInt(multiply(_lower_boundary.c_str(), _s2))<=BigInt(_s1) &&
            BigInt(multiply(add(_lower_boundary.c_str(), "1").c_str(), _s2))>BigInt(_s1))
            break;
        std::string tmp = divide_by_two(add(_upper_boundary.c_str(), _lower_boundary.c_str()));
        if (BigInt(multiply(tmp.c_str(), _s2))<=BigInt(_s1)){
            _lower_boundary = tmp;
        }
        else{
            _upper_boundary = tmp;
        }
    }
    return _lower_boundary;
}

BigInt::BigInt(): str("0") {};

BigInt::BigInt(const std::string& _str): str(_str) {}

BigInt::BigInt(const char* _ch): str(std::string(_ch)) {}

BigInt::BigInt(long long _i): str(std::to_string(_i)) {}

//comparison operators
bool BigInt::operator<(const BigInt& _bi) const {
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

bool BigInt::operator>(const BigInt& _bi) const { return _bi < *this; }

bool BigInt::operator==(const BigInt& _bi) const { return !(*this > _bi || *this < _bi); }

bool BigInt::operator!=(const BigInt& _bi) const { return *this > _bi || *this < _bi; }

bool BigInt::operator<=(const BigInt& _bi) const { return !(*this > _bi); }

bool BigInt::operator>=(const BigInt& _bi) const { return !(*this < _bi); }

//arithmetic operations
BigInt& BigInt::operator+=(const BigInt& _bi){
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

BigInt BigInt::operator+(const BigInt& _bi) const {
    BigInt res = *this;
    return res+=_bi;
}

BigInt& BigInt::operator++(){ return *this+=1; }

BigInt BigInt::operator++(int){
    BigInt res = *this;
    res+=1;
    return res;
}

BigInt BigInt::operator+()
{
    BigInt res = *this;
    return res;
}

BigInt& BigInt::operator-=(const BigInt& _bi){
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

BigInt BigInt::operator-(const BigInt& _bi){
    BigInt res = *this;
    return res -= _bi;
}

BigInt& BigInt::operator--(){ return *this-=1; }

BigInt BigInt::operator--(int){
    BigInt res = *this;
    res += 1;
    return res;
}

BigInt BigInt::operator-()
{
    BigInt res = *this * -1;
    return res;
}

BigInt& BigInt::operator*=(const BigInt& _bi){
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

BigInt BigInt::operator*(const BigInt& _bi){
    BigInt res = *this;
    return res *= _bi;
}

BigInt& BigInt::operator/=(const BigInt& _bi){
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

BigInt BigInt::operator/(const BigInt& _bi){
    BigInt res = *this;
    return res /= _bi;
}

BigInt& BigInt::operator%=(const BigInt& _bi){
    if (_bi == "0"){
        *this = "";
        return *this;
    }
    *this = *this - (*this/_bi)*_bi;
    return *this;
}

BigInt BigInt::operator%(const BigInt& _bi){
    BigInt res = *this;
    return res %= _bi;
}

//input and output operators
std::istream& operator>>(std::istream& input, BigInt& _bi){
    char tmp[INPUT_BUFFER_SIZE];
    input >> std::setw(INPUT_BUFFER_SIZE) >> tmp;
    _bi = tmp;
    return input;
}

std::ostream& operator<<(std::ostream& output, const BigInt& _bi){
    output << _bi.str;
    return output;
}

// static public functions
BigInt BigInt::abs(const BigInt& bi){
    BigInt res = bi;
    if (res.str[0] == '-'){
        res.str.erase(0, 1);
    }
    return res;
}

BigInt BigInt::pow(const BigInt& base, const BigInt& power){
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
