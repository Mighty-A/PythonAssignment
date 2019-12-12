#include <string>
#include "BigInteger.h"
#include <iostream>
//**********************************
// implementation of class BigInteger

BigInteger operator+(const BigInteger& a, const BigInteger& b)
{
    BigInteger tmp, tmpa = a, tmpb = b;
    if (a.is_positive && b.is_positive) {
        tmp.is_positive = true;
        bool flag = false;
        BigInteger *bigger, *smaller;
        if (a.num.length() < b.num.length()) {
            bigger = &tmpb;
            smaller = &tmpa;
        } else {
            bigger = &tmpa;
            smaller = &tmpb;
        }
        for (int i = 0; i <= smaller->num.length() - 1; i++) {
            bigger->num[i] += smaller->num[i] - '0';
        }
        for (int i = 0; i < bigger->num.length() - 1; i++) {
            bigger->num[i + 1] = bigger->num[i + 1] + (bigger->num[i] - '0') / 10;
            bigger->num[i] = '0' + (bigger->num[i] - '0') % 10;
        }
        if (bigger->num[bigger->num.length() - 1] - '0' >= 10) {
            bigger->num[bigger->num.length() - 1] = '0' + (bigger->num[bigger->num.length() - 1] - '0') % 10; 
            bigger->num += '1';
        }
        return *bigger;
    } else if (!a.is_positive && !b.is_positive) {
        tmpa.is_positive = true;
        tmpb.is_positive = true;
        tmp = tmpa + tmpb;
        tmp.is_positive = false;
        return tmp;
    } else if (a.is_positive) {
        tmp = b;
        tmp.is_positive = true;
        return (a - tmp);
    } else {
        tmp = a;
        tmp.is_positive = true;
        return (b - tmp);
    }
}

BigInteger operator-(const BigInteger& a, const BigInteger& b) {
    BigInteger tmp, tmpa = a, tmpb = b;
    if (tmpa.is_positive && tmpb.is_positive) {
        //compare which is bigger, and minus the bigger one with the smaller one
        BigInteger *bigger, *smaller;
        if (tmpa == tmpb) {
            tmp.num += '0';
            tmp.is_positive = true;
            return tmp;
        } else if (tmpa > tmpb) {       //minus a with b
            for (int i = tmpb.num.length(); i < tmpa.num.length(); i++) {
                tmpb.num +='0';
            }
            tmp.is_positive = true;
            int flag = 0;           //
            for (int i = 0;i < tmpa.num.length(); i++) {
                if (tmpb.num[i] > tmpa.num[i] - flag || tmpa.num[i] - flag - '0' < 0) {
                    tmp.num += tmpa.num[i] + 10 - tmpb.num[i] - flag + '0';
                    flag = true;
                } else {
                    tmp.num += tmpa.num[i] - tmpb.num[i] - flag + '0';
                    flag = false;
                }
            }
            while (tmp.num.length() > 1 && tmp.num.back() == '0')          //remove the possible zero at the front
                tmp.num.pop_back();
            return tmp;
        } else {
            tmp = tmpb - tmpa;
            tmp.is_positive = false;
            return tmp;
        }
    } else if (!tmpa.is_positive && !tmpb.is_positive) {
        tmpb.is_positive = true;
        tmpa.is_positive = true;
        tmp = tmpb - tmpa;
        return tmp;
    } else if (tmpa.is_positive) {
        tmpb.is_positive = true;
        tmp = tmpa + tmpb;
        return tmp;
    } else if (tmpb.is_positive) {
        tmpa.is_positive = true;
        tmp = tmpa + tmpb;
        tmp.is_positive = false;
        return tmp;
    }
}

BigInteger operator*(const BigInteger& a, const BigInteger& b) {
    BigInteger tmp, tmpa = a, tmpb = b;
    if (a.is_positive && b.is_positive || !a.is_positive && !b.is_positive) {       //the result is positive
        tmp.is_positive = true;
        int len1 = tmpa.num.length();
        int len2 = tmpb.num.length();
        int len = len1 + len2 + 1;
        int *num1 = new int[len1];
        int *num2 = new int[len2];
        int *ans = new int [len];
        for (int i = 0; i < len1; i++) {
            num1[i] = tmpa.num[i] - '0';
        }
        for (int i = 0; i < len2; i++) {
            num2[i] = tmpb.num[i] - '0';
        }
        for (int i = 0; i < len; i++) {
            ans[i] = 0;
        }
        for (int i = 0; i < len1; i++) {
            for (int j = 0; j < len2; j++) {
                ans[i + j] += num1[i] * num2[j];
                ans[i + j + 1] += ans[i + j] / 10;
                ans[i + j] = ans[i + j] % 10;
            }
        }
        int i = len - 1;
        while (ans[i] == 0 && i > 0) {
            i--;
        }
        for (int j = 0; j <= i; j++) {
            tmp.num += ans[j] + '0';
        }
        delete [] num1;
        delete [] num2;
        delete [] ans;
        return tmp;
    } else if (a.is_positive) {                                                     //the result is negative
        tmpb.is_positive = true;
        tmp = tmpa * tmpb;
        tmp.is_positive = false;
        return tmp;
    } else {
        tmpa.is_positive = true;
        tmp = tmpa * tmpb;
        tmp.is_positive = false;
        return tmp;
    }
}

bool IsGreater(int* num1, int* num2, int len)       //it's a tool func used by "/"
{
    if (num1[len] != 0)
        return true;
    for (int i = len - 1; i >= 0; i--) {
        if (num1[i] < num2[i])
            return false;
        else if (num1[i] > num2[i]) 
            return true;
    }
    return true;
}
BigInteger operator/(const BigInteger& a, const BigInteger& b) {
    BigInteger tmp, tmpa = a, tmpb = b;
    if (tmpa.num.length() == 1 && tmpa.num[0] == '0') {
        tmp.is_positive = true;
        tmp.num += '0';
        return tmp;
    }
    if (a.is_positive && b.is_positive || !a.is_positive && !b.is_positive) {
        if (a < tmpb) {
            tmp.is_positive = true;
            tmp.num += '0';
            return tmp;
        } else if (a == tmpb) {
            tmp.is_positive = true;
            tmp.num += '1';
            return tmp;
        } else {                    //a > b
            int len1 = a.num.length();
            int len2 = b.num.length();
            int* num1 = new int [len1 + 1];
            int* num2 = new int [len2];
            int* ans = new int [len1];
            for (int i = 0; i < len1; i++) {
                num1[i] = a.num[i] - '0';
                ans[i] = 0;
            }
            num1[len1] = 0;          //avoid oversteping the boundary of the array
            for (int i = 0; i < len2; i++)
                num2[i] = b.num[i] - '0';
            for (int i = len1 - len2; i >= 0; i--) {        //align
                while (IsGreater(num1 + i, num2, len2)) {
                    for (int j = 0; j < len2; j++) {
                        num1[i + j] -= num2[j];
                        if (num1[i + j] < 0) {
                            num1[i + j + 1] -= 1;
                            num1[i + j] += 10;
                        }
                    }
                    ans[i] += 1;
                }
            }
            int i = len1 - 1;
            while (ans[i] == 0 && i > 0) 
                i--;
            for (int j = 0; j <= i; j++) 
                tmp.num += ans[j] + '0';
            tmp.is_positive = true;
            delete [] num1;
            delete [] num2;
            delete [] ans;
            return tmp;
        }
    } else {
        
        tmpa.is_positive = true;
        tmpb.is_positive = true;
        tmp = tmpa / tmpb;
        tmp.is_positive = false;
        BigInteger one("1");
        one.is_positive = true;
        tmp = tmp - one;
        return tmp;
    }
}

BigInteger operator%(const BigInteger& a, const BigInteger& b) {
    return (a -  (a / b) * b);
}


std::ostream& operator<<(std::ostream& os, const BigInteger& a) {
    if (!a.is_positive && !(a.num.length() == 1 && a.num[0] == '0'))
        std::cout << '-';
    for (int i = a.num.length() - 1; i >= 0; i--)
        std::cout << a.num[i];
    return os;
}