/*************************************************************************
	> File Name: strchar.h
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2023年05月17日 星期三 11时15分55秒
 ************************************************************************/

#include<iostream>
#include <string>
#include <vector>
using namespace std;
vector<unsigned char> strToUCharArr(string s) {
    vector<unsigned char> res;
    int n = s.size();
    int cnt = 0;
    unsigned char cur = 0;
    for (int i = 0; i < n; i++) {
        if (cnt == 8) {
            res.push_back(cur);
            cur = 0;
            cnt = 0;
        }
        cur <<= 1;
        cur |= s[i] - '0';
        cnt++;
    }
    if (cnt > 0) {
        cur <<= (8 - cnt);
        res.push_back(cur);
        //cnt = 8 - cnt;
    }
    res.push_back(cnt);
    return res;
}


string ucharArrToStr(vector<unsigned char> v) {
    int n = v.size() - 1;
    int cnt = v[n];
    string res = "";
    for (int i = 0; i < n; i++) {
        unsigned char cur = v[i];
        for (int j = 7; j >= 0; j--) {
            if (i == n - 1 && (8-j) > cnt) {
                break;
            }
            if (cur & (1 << j)) {
                res += "1";
            } else {
                res += "0";
            }
        }
    }
    return res;
}
