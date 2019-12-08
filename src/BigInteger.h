#ifndef BIGINTEGER
#define BIGINTEGER
#include <iostream>
#include <string>
class BigInteger {
    
    friend BigInteger operator+(const BigInteger&, const BigInteger&);
    friend BigInteger operator*(const BigInteger&, const BigInteger&);
    friend BigInteger operator-(const BigInteger&, const BigInteger&);
    friend BigInteger operator/(const BigInteger&, const BigInteger&);      //actually, it means "//" rather than "/" in the codes
    friend BigInteger operator%(const BigInteger&, const BigInteger&);
    friend std::ostream& operator<<(std::ostream& , const BigInteger&);
private:
    std::string num; //stored in reverse order
    bool is_positive = true;
    
public:
    BigInteger() {
    }
    BigInteger(std::string numString) //constructor from the TerminalNode
    {
        if (numString[0] == '-') {
            is_positive = false;
            for (int i = 1; i < numString.length(); i++) {
                num += numString[numString.length() - i];
            }
        } else {
            is_positive = true;
            for (int i = 1; i <= numString.length(); i++)  {
                num += numString[numString.length() - i];
            }
        }
    }
    //Display :output
    void Display()
    {
        if (!is_positive && !(num.length() == 1 && num[0] == '0'))
            std::cout << '-';
        for (int i = num.length() - 1; i >= 0; i--)
            std::cout << num[i];
        return;
    }

    //overload of comparison
    bool operator<(const BigInteger& a) const {
        if (a.is_positive && is_positive) {
            if (num.length() < a.num.length())
                return true;
            else if (num.length() > a.num.length())
                return false;
            else {
                for (int i = num.length() - 1; i >= 0; i--) {
                    if (num[i] < a.num[i]) 
                        return true;
                    else if (num[i] > a.num[i])
                        return false;
                }
                return false;
            }
        } else if (!is_positive && !a.is_positive) {
            if (num.length() < a.num.length())
                return false;
            else if (num.length() > a.num.length())
                return true;
            else {
                for (int i = num.length() - 1; i >= 0; i--) {
                    if (num[i] < a.num[i])
                        return false;
                    else if (num[i] > a.num[i])
                        return true;
                }
                return false;
            }
        } else if (a.is_positive) {
            return true;
        } else if (is_positive) {
            return false;
        }
    }
    bool operator==(const BigInteger& a) const {
        if (is_positive == a.is_positive) {
            if (a.num.length() == num.length()) {
                for (int i = num.length() - 1; i >= 0; i--) {
                    if (num[i] != a.num[i])
                        return false;
                }
                return true;
            }
            else 
                return false;
        } else 
            return false;
    }
    bool operator>(const BigInteger& a) const{
        if (*this == a || *this < a) 
            return false;
        else 
            return true;
    }
    bool operator>=(const BigInteger& a) const{
        return !(*this < a);
    }
    bool operator<=(const BigInteger& a) const{
        return !(*this > a);
    }
    bool operator!=(const BigInteger& a) const{
        return !(*this == a);
    }
    BigInteger& operator+=(BigInteger& a) {
        *this = *this + a;
        return *this;
    }
    BigInteger& operator-=(BigInteger& a) {
        *this = *this - a;
        return *this;
    }
    BigInteger& operator*=(BigInteger& a) {
        *this = *this * a;
        return *this;
    }
    BigInteger& operator/=(BigInteger& a) {
        *this = *this / a;
        return *this;
    }
    
};

#endif