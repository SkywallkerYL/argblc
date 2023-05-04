/*************************************************************************
	> File Name: haflmantest.cpp
	> Author: yangli
	> Mail: 577647772@qq.com 
	> Created Time: 2023年05月04日 星期四 18时15分38秒
 ************************************************************************/

#include<iostream>
#include "halfman.h"
using namespace std;
int main(){
	char pixel [] = "hhhhhiiiiiinwndaubuedknjnksaxbaweakdskl";
    string str(pixel);// = "hello world"; // 原始字符串
    unordered_map<uint8_t, string> code_map = huffmanEncode(str); // 哈夫曼编码
    string encoded_str = "";
    for(uint8_t c : str){
        encoded_str += code_map[c]; // 将编码后的字符拼接成字符串
    }
    HuffmanNode* root = buildHuffmanTree(str); // 构建哈夫曼树
    string decoded_str = huffmanDecode(encoded_str, root); // 哈夫曼解码
    // 输出结果
    cout << "Original string: " << str << endl;
    cout << "Encoded string: " << encoded_str << endl;
    cout << "Decoded string: " << decoded_str << endl;

	return 0 ;
}
