/*************************************************************************
	> File Name: strToCharTest.cpp
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2023年05月16日 星期二 20时12分01秒
 ************************************************************************/

#include<iostream>
#include "strchar.h"
using namespace std;
int main() {
    string s = "1111111001011011001101101001010001100";
    vector<unsigned char> v = strToUCharArr(s);
	cout << s.size() <<" "<< v.size()<<endl;
    for (int i = 0; i < v.size(); i++) {
        cout << (int)v[i] << " ";
    }
    cout << endl;
    string t = ucharArrToStr(v);
	cout << s << endl;
    cout << t << endl;
    return 0;
}

