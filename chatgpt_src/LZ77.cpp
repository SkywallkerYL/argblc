#include <iostream>
#include <string>
#include <vector>
/*
首先定义了一个Triple结构体，表示LZ77算法中的三元组，其中pos表示相对于当前位置的偏移量，len表示匹配的长度，next表示下一个字符。
接着定义了一个名为lz77的函数，用于实现LZ77算法。函数的参数为一个字符串str，函数的返回值为一个存储了LZ77算法三元组的向量triples。
在函数中，首先初始化了当前位置i为0，并且创建了一个空的三元组向量triples用于存储结果。
然后进入一个循环，循环条件为i < str.length()，即当前位置不能超过字符串的长度。
在循环中，首先初始化了一个三元组triple，其中pos和len都为0，next为当前字符。
然后从当前位置i向左依次尝试匹配前面的字符，匹配的起始位置为j，初始值为i-1。同时限制匹配的长度不超过255个字符。
在匹配过程中，首先初始化当前匹配长度len为0，然后从当前位置开始依次比较两个字符串中的字符是否相等，如果相等则增加匹配长度len。
如果当前匹配长度len大于之前的最大匹配长度maxLen，则更新三元组中的pos和len，并更新最大匹配长度maxLen。
最后将匹配到的三元组triple添加到结果向量triples中，并更新当前位置i为匹配到的位置加上匹配长度再加上1。
循环结束后，函数返回存储了所有三元组的向量triples。
最后在main函数中，定义了一个字符串str并初始化为"ababcbabc"，然后调用lz77函数将字符串压缩成LZ77算法的三元组，最后输出所有三元组的内容。



*/
using namespace std;

// 定义LZ77算法中的三元组结构体
struct Triple {
    int pos;  // 相对于当前位置的偏移量
    int len;  // 匹配的长度
    char next;  // 下一个字符
};

// 实现LZ77算法的函数
vector<Triple> lz77(string str) {
    vector<Triple> triples;  // 存储LZ77算法的三元组
    int i = 0;  // 当前位置
    while (i < str.length()) {
        Triple triple = {0, 0, str[i]};  // 初始化三元组
        int j = i - 1;  // 匹配的起始位置
        int maxLen = 0;  // 最大匹配长度
        while (j >= 0 && i - j <= 255) {  // 最多匹配255个字符
            int len = 0;  // 当前匹配长度
            while (i + len < str.length() && str[j + len] == str[i + len]) {
                len++;
            }
            if (len > maxLen) {
                triple.pos = i - j - len;  // 记录偏移量
                triple.len = len;  // 记录匹配长度
                maxLen = len;  // 更新最大匹配长度
            }
            j--;
        }
        triples.push_back(triple);  // 将三元组添加到结果中
        i += maxLen + 1;  // 更新当前位置
    }
    return triples;
}

int main() {
    string str = "ababcbabc";
    vector<Triple> triples = lz77(str);
    for (int i = 0; i < triples.size(); i++) {
        cout << "<" << triples[i].pos << "," << triples[i].len << "," << triples[i].next << ">" << endl;
    }
    return 0;
}
